import matplotlib.pyplot as plt
import matplotlib.patches as patches
import yarp

#Foot measurements (cm)
length = 24
width = 12.5
separation = 12

#Initialise YARP
yarp.Network.init()

#Create a YARP port and connect it to the output port
input_port = yarp.Port()
input_port.open("/reader")
yarp.Network.connect("/sender", "/reader")

#Create figure
fig = plt.figure()

while True:
        #Plot rectangles in figure
        ax = fig.add_subplot(111, aspect='equal')
        for p in [patches.Rectangle((-(width+separation/2), -(length/2)), width, length, hatch='/', fill=False),
                  patches.Rectangle((separation/2, -(length/2)), width, length, hatch='\\', fill=False)]:
            ax.add_patch(p)
     
        #Read the data from the port
        output = yarp.Bottle()
        input_port.read(output)
        x=output.get(0).asDouble()
        y=output.get(1).asDouble()

        #Transformation from robot's axes coordinates to common coordinates
        x2=-y
        y2=x

        #Plot point in figure
        for c in [patches.Circle((x2, y2), 1)]:
            ax.add_patch(c)
        
        #Axes configuration
        plt.axis([-30, 30, -30, 30])
        plt.xlabel('X (cm)')
        plt.ylabel('Y (cm)')
        
        #Show figure and refresh
        plt.show()
        plt.pause(0.1)
        fig.clf()
        


