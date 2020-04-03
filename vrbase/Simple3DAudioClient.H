/**
 * /author Bret Jackson
 *
 * /file  Simple3DAudioClient.H
 * /brief 
 *
 */ 

#ifndef SIMPLE3DAUDIOCLIENT_H
#define SIMPLE3DAUDIOCLIENT_H

#include "CommonInc.H"

namespace VRBase {

typedef G3D::ReferenceCountedPointer<class Simple3DAudioClient> Simple3DAudioClientRef;

/** This class is a simple client that is used to create a TCP connection with
	a Max/MSP server that controls audio output to an eight speaker cube for 3D
	sound effects based on listener and source position.
*/

class Simple3DAudioClient : public G3D::ReferenceCountedObject
{
public:
	Simple3DAudioClient(string host, int port);
	virtual ~Simple3DAudioClient();
	virtual void loadFile(string filename, int id);
	virtual void play(int id, bool loop = false, float startTime = 0, float endTime = -1);
	virtual void resume(int id);
	virtual void pause(int id);
	virtual void stop(int id);
	virtual void seek(int id, float time);
	virtual void setAudioPos(int id, const G3D::CoordinateFrame &posInVirtual);
	virtual void setListenerPos(const G3D::CoordinateFrame &posInRoom);
	virtual void setVirtualToRoomSpace(const G3D::CoordinateFrame &virtualToRoomSpace);
	virtual void setGain(int id, float val);
	virtual void setMasterGain(float val);

protected:
	G3D::ReliableConduitRef		_conduit;
	G3D::NetworkDevice*			_netDevice;

	void sendMessage(string message);
};

class AudioMessage
{
public:
	AudioMessage() {};
	AudioMessage(string mess) { message = mess; };
	~AudioMessage() {};
	void serialize(G3D::BinaryOutput &b) const { b.writeString(message); };

	string message;
};

}// end namespace VRBase
#endif