# internetoverlay
Should have become a light-weight replacement for the Tor network.
I don't know if it will compile or even work properly. These sources rot since some years in my archive.
My intention was to develop a minimalistic replacement for the Tor network as implementing Tor on a microcontroller is almost impossible.
InternetOverlay works by tunneling tiny, encrypted messages through a mesh of clients randomly. Only the receiver can decrypt messages but does not tell anyone if HE is actually the receiver and still forwards the message to other clients. 
There is no path-searching algorithms or such, it's pure 'luck' that the message arrives. This has 2 advantages: no algorithms that can be exploited, nobody knows that you are the real receiver. Disadvantages: everyone knows that you are part of the network (your ip) and that you *might* be a message receiver.
There is a chat example, written with Qt, that worked! It has sent and received chat messages. My next step was to implement a protocol which ensures that packages will arrive. Just like TCP.
I know that there is a serious problem with my protocol structure. I use union sturcts to serialize data. So different endians will not work together.
Sounds interesting? I will not abandon this project, it still sounds very nice and I like the idea. If you want to work together with me on this project, give me a message. I would LOVE to continue this project.
