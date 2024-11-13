#include <iostream>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QTextStream>
#include <QDebug>
#include <QString>

using namespace std;

const int MAX_ITER = 100000;
const double GM = 398600441800000.0; // Earth standard gravitational parameter
const double mu = 398600.4418; // Earth's gravitational parameter [km³/s²]

struct Vector3 {
    double x, y, z;
};

Vector3 cross(const Vector3& a, const Vector3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

double dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double norm(const Vector3& vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

double deg_to_rad(double x) {
    return x * M_PI / 180.0;
}

double rad_to_deg(double x) {
    return x * 180.0 / M_PI;
}

void kepler_to_state(double a, double e, double i, double Omega, double w, double M) {
    // Convert  Keplerian elements' angles to radians
    w = deg_to_rad(w);
    Omega = deg_to_rad(Omega);
    i = deg_to_rad(i);
    M = deg_to_rad(M);

    // Newton's method solve of Kepler's equation for eccentric anomaly
    double E_last = 0, E = M;
    int iter = 0;
    while (E_last != E) {
        if (iter > MAX_ITER) {
            cout << "ERROR: solution does not converge! Check inputs and try again." << endl;
            return;
        }
        E_last = E;
        E = E_last - (E_last - e * sin(E_last) - M) / (1 - e * cos(E_last));
        iter++;
    }

    // True anomaly
    double nu = 2 * atan2(sqrt(1 + e) * sin(0.5 * E), sqrt(1 - e) * cos(0.5 * E));

    // Distance to Earth center
    double r_c = a * (1 - e * cos(E));

    // Orbital frame
    Vector3 r_o = {r_c * cos(nu), r_c * sin(nu), 0.0};
    Vector3 v_o = {
        -sqrt(GM * a) / r_c * sin(E),
        sqrt(GM * a) / r_c * sqrt(1 - e * e) * cos(E),
        0.0
    };

    // Convert to ECI frame
    Vector3 r = {
        r_o.x * (cos(w) * cos(Omega) - sin(w) * cos(i) * sin(Omega)) - 
        r_o.y * (sin(w) * cos(Omega) + cos(w) * cos(i) * sin(Omega)),
        
        r_o.x * (cos(w) * sin(Omega) + sin(w) * cos(i) * cos(Omega)) + 
        r_o.y * (cos(w) * cos(i) * cos(Omega) - sin(w) * sin(Omega)),
        
        r_o.x * sin(w) * sin(i) + r_o.y * cos(w) * sin(i)
    };

    Vector3 v = {
        v_o.x * (cos(w) * cos(Omega) - sin(w) * cos(i) * sin(Omega)) - 
        v_o.y * (sin(w) * cos(Omega) + cos(w) * cos(i) * sin(Omega)),
        
        v_o.x * (cos(w) * sin(Omega) + sin(w) * cos(i) * cos(Omega)) + 
        v_o.y * (cos(w) * cos(i) * cos(Omega) - sin(w) * sin(Omega)),
        
        v_o.x * sin(w) * sin(i) + v_o.y * cos(w) * sin(i)
    };

    // Print results
    cout << "\nSemi-major Axis: " << a << " m" << endl;
    cout << "Eccentricity: " << e << endl;
    cout << "Argument of Periapsis: " << rad_to_deg(w) << "°" << endl;
    cout << "Longitude of Ascending Node: " << rad_to_deg(Omega) << "°" << endl;
    cout << "Inclination: " << rad_to_deg(i) << "°" << endl;
    cout << "True Anomaly: " << rad_to_deg(nu) << "°" << endl;
    cout << "Mean Anomaly: " << rad_to_deg(M) << "°" << endl;
    cout << "Eccentric Anomaly: " << rad_to_deg(E) << "°" << endl;
    cout << "Position Vector: " << r.x << ", " << r.y << ", " << r.z << " m" << endl;
    cout << "Velocity Vector: " << v.x << ", " << v.y << ", " << v.z << " m/s\n" << endl;
}

struct KeplerianElements {
    double inclination;
    double raan;
    double eccentricity;
    double argumentOfPerigee;
    double meanAnomaly;
    double meanMotion;
    double a;
};

KeplerianElements parseTLE(const QString& line1, const QString& line2) {
    KeplerianElements elements;
    
    elements.inclination = line2.mid(8, 8).toDouble();
    elements.raan = line2.mid(17, 8).toDouble();
    elements.eccentricity = line2.mid(26, 7).toDouble() * 1e-7;  // Convert to proper format
    elements.argumentOfPerigee = line2.mid(34, 8).toDouble();
    elements.meanAnomaly = line2.mid(43, 8).toDouble();
    elements.meanMotion = line2.mid(52, 11).toDouble() * (2.0 * M_PI / 86400.0); // Convert to rad/s

    elements.a = pow(mu / (elements.meanMotion * elements.meanMotion), 1.0/3.0);
    
    return elements;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cout << "" << endl;
    cout << "===================================================" << endl;
    cout << "RENDEZ-VOUS WITH ISS" << endl;
    cout << "Authors: Recep Kamrak, Emil Mammadli, Burak Saglam" << endl;
    cout << "Middle East Technical University" << endl;
    cout << "===================================================" << endl;

    // Create data directory
    QDir().mkpath("./data");

    QNetworkAccessManager manager;
    QEventLoop loop;

    QNetworkRequest request(QUrl("https://live.ariss.org/iss.txt"));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() == QNetworkReply::NoError) {
        QFile file("./data/iss_tle.txt");
        if(file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            cout << "[Debug] TLE ile downloaded successfully!" << endl;
        }
    } else {
        cout << "[DEBUG] Download failed: " << reply->errorString().toStdString() << endl;
    }

    cout << "[DEBUG] Parsing TLE..." << endl;

    QFile file("./data/iss_tle.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line0 = in.readLine(); // Satellite name
        QString line1 = in.readLine(); // TLE line 1
        QString line2 = in.readLine(); // TLE line 2
        
        KeplerianElements elements = parseTLE(line1, line2);
        
        // cout << "Keplerian Elements:" << endl;
        // cout << "Inclination (i): " << elements.inclination << " deg" << endl;
        // cout << "RAAN (Omega): " << elements.raan << " deg" << endl;
        // cout << "Eccentricity (e): " << elements.eccentricity << endl;
        // cout << "Argument of Perigee (w): " << elements.argumentOfPerigee << " deg" << endl;
        // cout << "Mean Anomaly (M): " << elements.meanAnomaly << " deg" << endl;
        // cout << "Mean Motion: " << elements.meanMotion << " revs/day" << endl;
        // cout << "Semi-major Axis (a): " << elements.a << " m" << endl;

        double a = elements.a;
        double i = elements.inclination;
        double e = elements.eccentricity;
        double Omega = elements.raan;
        double w = elements.argumentOfPerigee;
        double M = elements.meanAnomaly;

        kepler_to_state(a,e,i,Omega,w,M);

        
        file.close();
    }

    return 0;
}
