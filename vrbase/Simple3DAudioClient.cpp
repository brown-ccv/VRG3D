/**
 * /author Bret Jackson
 *
 * /file  Simple3DAudioClient.cpp
 * /brief 
 *
 */ 

#include "Simple3DAudioClient.H"

namespace VRBase {

using namespace G3D;

Simple3DAudioClient::Simple3DAudioClient(string host, int port)
{
	_netDevice = NetworkDevice::instance();
	if (_netDevice == NULL) {
		cerr << "ERROR: unable to create network device in Simple3DAudioClient constructor"<<endl;
	}
	NetAddress address(host, port);
	_conduit = ReliableConduit::create(address);
	if (!_conduit->ok()) {
		cerr << "ERROR: unable to create tcp connection to " << host << " on port " << port << endl;
	}
}

Simple3DAudioClient::~Simple3DAudioClient()
{
	NetworkDevice::cleanup();
}

void
Simple3DAudioClient::sendMessage(string message)
{
	AudioMessage m(message);
	if (_conduit->ok()) {
		_conduit->send(1, m);
	}
	else {
		cout << "Error sending audio message. Conduit is not ok" << endl;
	}
}

void
Simple3DAudioClient::loadFile(string filename, int id)
{
	stringstream message;
	message << "loadfile " << id << " " << filename;
	sendMessage(message.str());
}

void
Simple3DAudioClient::play(int id, bool loop/* = false*/, float startTime/* = 0*/, float endTime/* = -1*/)
{
	stringstream message;
	message << "play " << id << " ";
	if (loop) {
		message << 1;
	}
	else {
		message << 0;
	}
	message << " " << startTime;
	if (endTime != -1 && endTime > startTime) {
		message << " " << endTime;
	}
	sendMessage(message.str());
}

void
Simple3DAudioClient::resume(int id)
{
	stringstream message;
	message << "resume " << id;
	sendMessage(message.str());
}

void
Simple3DAudioClient::pause(int id)
{
	stringstream message;
	message << "pause " << id;
	sendMessage(message.str());
}

void
Simple3DAudioClient::stop(int id)
{
	stringstream message;
	message << "stop " << id;
	sendMessage(message.str());
}

void
Simple3DAudioClient::seek(int id, float time)
{
	stringstream message;
	message << "seek " << id << " " << time;
	sendMessage(message.str());
}

void
Simple3DAudioClient::setAudioPos(int id, const CoordinateFrame &posInVirtual)
{
	stringstream message;
	message << "setaudiopos " << id << " " << posInVirtual;
	sendMessage(message.str());
}

void
Simple3DAudioClient::setListenerPos(const CoordinateFrame &posInRoom)
{
	stringstream message;
	message << "setlistenerpos " << posInRoom;
	sendMessage(message.str());
}

void
Simple3DAudioClient::setVirtualToRoomSpace(const CoordinateFrame &virtualToRoomSpace)
{
	stringstream message;
	message << "setvirtualtoroomspace " << virtualToRoomSpace;
	sendMessage(message.str());
}
	
void
Simple3DAudioClient::setGain(int id, float val)
{
	stringstream message;
	message << "setgain " << id << " " << val;
	sendMessage(message.str());
}

void
Simple3DAudioClient::setMasterGain(float val)
{
	stringstream message;
	message << "setmastergain " << val;
	sendMessage(message.str());
}


} // end namespace
