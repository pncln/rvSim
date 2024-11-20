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
#include <cmath>
#include <stdexcept>
#include <string>
#include <tuple>

#include "transform2d.h"

using namespace std;

const int MAX_ITER = 100000;
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

std::tuple<Vector3, Vector3> generateOrbit(double x_km, double y_km, double inclination) {
    // Earth standard gravitational parameter [m^3/s^2]
    const double GM = 398600441800000.0;

    // Earth radius [m]
    const double R = 6371000.0;

    const double M_PER_KM = 1000.0;
    const double MIN_ALT = 100;
    const double MAX_ALT = 1000;

    // Input validation
    if (x_km < MIN_ALT || y_km < MIN_ALT) {
        throw std::runtime_error("Altitudes must be " + std::to_string(MIN_ALT) + " km or above");
    }

    if (x_km > MAX_ALT || y_km > MAX_ALT) {
        // Warning could be handled differently based on needs
        // Currently just continuing execution
    }

    if (inclination < -90.0 || inclination > 90.0) {
        throw std::runtime_error("Inclination must be between -90 and 90 degrees");
    }

    // Convert to meters
    double x_r = R + M_PER_KM * x_km;
    double y_r = R + M_PER_KM * y_km;

    // Initial position vector
    Vector3 init_pos = {x_r, 0.0, 0.0};

    // Calculate semi-major axis
    double a = 0.5 * (x_r + y_r);

    // Calculate initial velocity using vis-viva equation
    double init_vel_mag = std::sqrt(GM * (2.0/x_r - 1.0/a));

    // Convert inclination to radians for trig functions
    double inc_rad = inclination * M_PI / 180.0;
    Vector3 init_vel = {0.0,
                        init_vel_mag * std::cos(inc_rad),
                        init_vel_mag * std::sin(inc_rad)};

    return {init_pos, init_vel};
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
        -sqrt(mu * a) / r_c * sin(E),
        sqrt(mu * a) / r_c * sqrt(1 - e * e) * cos(E),
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

QString getTLEDate(const QString& line1) {
    int year = line1.mid(18, 2).toInt();
    double dayOfYear = line1.mid(20, 12).toDouble();
    
    year = (year < 57) ? year + 2000 : year + 1900;
    
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        daysInMonth[1] = 29;  // Leap year
    }
    
    int day = (int)dayOfYear;
    int month = 1;
    while (day > daysInMonth[month-1]) {
        day -= daysInMonth[month-1];
        month++;
    }
    
    double fractionalDay = dayOfYear - floor(dayOfYear);
    int hours = (int)(fractionalDay * 24);
    int minutes = (int)((fractionalDay * 24 - hours) * 60);
    int seconds = (int)(((fractionalDay * 24 - hours) * 60 - minutes) * 60);
    
    return QString("%1-%2-%3 %4:%5:%6")
        .arg(year)
        .arg(month, 2, 10, QChar('0'))
        .arg(day, 2, 10, QChar('0'))
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

double dateToTAIMJD(int year, int month, int day, int hour, int minute, int second) {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    
    double jd = day + (hour - 12.0) / 24.0 + minute / 1440.0 + second / 86400.0
              + (153 * m + 2) / 5 
              + 365 * y 
              + y / 4 
              - y / 100 
              + y / 400 
              - 32045;
    
    double taiUtcOffset = 37.0 / 86400.0;
    double taiMjd = jd - 2430000.5 + taiUtcOffset;
    
    return taiMjd;
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
    cout << "==================== ISS DATA =====================";

    QFile file("./data/iss_tle.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line0 = in.readLine(); // Satellite name
        QString line1 = in.readLine(); // TLE line 1
        QString line2 = in.readLine(); // TLE line 2
        
        KeplerianElements elements = parseTLE(line1, line2);

        double a = elements.a;
        double i = elements.inclination;
        double e = elements.eccentricity;
        double Omega = elements.raan;
        double w = elements.argumentOfPerigee;
        double M = elements.meanAnomaly;

        kepler_to_state(a,e,i,Omega,w,M);

        QString date = getTLEDate(line1);
        cout << "TLE Epoch: " << date.toStdString() << endl;;
        
        QStringList parts = date.split(" ");
        QStringList dateParts = parts[0].split("-");
        QStringList timeParts = parts[1].split(":");

        int year = dateParts[0].toInt();
        int month = dateParts[1].toInt();
        int day = dateParts[2].toInt();
        int hour = timeParts[0].toInt();
        int minute = timeParts[1].toInt();
        int second = timeParts[2].toInt();

        double taiMjd = dateToTAIMJD(year, month, day, hour, minute, second);
        cout << "TAIMJD: " << taiMjd << endl;
        cout << "===================================================\n";

        std::array<double, 3> sat1_icrf = {1000, 2000, 500};  // km
        std::array<double, 3> sat2_icrf = {2000, 3000, 1000}; // km


        auto sat1_2d = TRANSFORM2D::transformTo2D(sat1_icrf, sat2_icrf, sat1_icrf);
        auto sat2_2d = TRANSFORM2D::transformTo2D(sat1_icrf, sat2_icrf, sat2_icrf);

        std::cout << "Satellite 1 2D coordinates: (" << sat1_2d[0] << ", " << sat1_2d[1] << ")\n";
        std::cout << "Satellite 2 2D coordinates: (" << sat2_2d[0] << ", " << sat2_2d[1] << ")\n";


        cout << "===================================================\n";
        
        file.close();
    }

    return 0;
}
