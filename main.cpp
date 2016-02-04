// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace std;
using namespace yarp::os;

//Origin of coordinates established in the middle point between the feet
#define Xcom 0 //Distance to COM in X axis [cm]
#define Ycom 0 //Distance to COM in Y axis [cm]
#define Zcom 103.6602 //Distance to COM in Z axis [cm]

int main()
{
    double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z, spd_x, spd_y, spd_z, mag_x, mag_y, mag_z;
    double x2,y2,z2;

    //Initialise YARP
    Network yarp;
    if (! Network::checkNetwork() )
    {
        printf("Please start a YARP server first.\n");
        return(-1);
    }

    //YARP port for reading from sensor
    BufferedPort<Bottle> readPort;
    readPort.open("/inertial:i");

    //YARP port for writing data
    Port writePort;
    writePort.open("/sender");

    Time::delay(2); //Wait for ports to open

    //Connect input and output ports
    Network::connect("/inertial","/inertial:i");
    Network::connect("/sender", "/reader");

    cout << "\nObtaining data from sensor...\n" << endl;

    while (true)
    {
        Bottle *input = readPort.read();

        if (input!=NULL)
        {
            //cout << "Full string: " << input->toString().c_str() << endl;
            for (int i=0; i<input->size(); i++)
            {
                // i = 0,1,2 --> euler angles (deg)
                // i = 3,4,5 --> linear acceleration (m/s²)
                // i = 6,7,8 --> angular speed (deg/s)
                // i = 9,10,11 --> magnetic field (arbitrary units)

                switch (i)
                {
                case 0:
                    ang_x=input->get(i).asDouble();
                    cout << "Angle X: " << ang_x << " deg" << endl;
                    break;
                case 1:
                    ang_y=input->get(i).asDouble();
                    cout << "Angle Y: " << ang_y << " deg" << endl;
                    break;
                case 2:
                    ang_z=input->get(i).asDouble();
                    cout << "Angle Z: " << ang_z << " deg" << endl;
                    break;
                case 3:
                    acc_x=input->get(i).asDouble();
                    cout << "Acceleration X: " << acc_x << " m/s²" << endl;
                    break;
                case 4:
                    acc_y=input->get(i).asDouble();
                    cout << "Acceleration Y: " << acc_y << " m/s²" << endl;
                    break;
                case 5:
                    acc_z=input->get(i).asDouble();
                    cout << "Acceleration Z: " << acc_z << " m/s²" << endl;
                    break;
                case 6:
                    spd_x=input->get(i).asDouble();
                    cout << "Angular speed X: " << spd_x << " deg/s" << endl;
                    break;
                case 7:
                    spd_y=input->get(i).asDouble();
                    cout << "Angular speed Y: " << spd_y << " deg/s" << endl;
                    break;
                case 8:
                    spd_z=input->get(i).asDouble();
                    cout << "Angular speed Z: " << spd_z << " deg/s" << endl;
                    break;
                case 9:
                    mag_x=input->get(i).asDouble();
                    cout << "Magnetic field X: " << mag_x << endl;
                    break;
                case 10:
                    mag_y=input->get(i).asDouble();
                    cout << "Magnetic field Y: " << mag_y << endl;
                    break;
                case 11:
                    mag_z=input->get(i).asDouble();
                    cout << "Magnetic field Z: " << mag_z << endl;
                    break;
                }
            }
            printf("\n");
        }

        //Transformation from sensor coordinates to robot coordinates
        x2 = -acc_x;
        y2 = acc_y;
        z2 = -acc_z;

        //Calculation of the Zero-Moment Point
        double Xzmp = Xcom - (Zcom / z2)*x2; //ZMP X coordinate [cm]
        double Yzmp = Ycom - (Zcom / z2)*y2; //ZMP Y coordinate [cm]
        printf("\nZMP = (%f, %f) cm\n\n", Xzmp, Yzmp);

        //Send ZMP coordinates through YARP
        Bottle send;
        send.addDouble(Xzmp);
        send.addDouble(Yzmp);
        writePort.write(send);

        Time::delay(1.0/4.0);
    }
    return 0;
}
