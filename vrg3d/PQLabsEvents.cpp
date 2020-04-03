#include "PQLabsEvents.h"

#include "G3DOperators.h"
#include <list>

namespace VRG3D {

using namespace G3D;
using namespace PQ_SDK_MultiTouch;

PQLabsEvents::PQLabsEvents(int port, string ip)
{
  _port = port;
  _ip = ip;
}

PQLabsEvents::PQLabsEvents( const std::string   name,
                                   Log     *log,
                        const ConfigMapRef  map )
{
  _port = map->get( name + "_Port", PQMT_DEFAULT_CLIENT_PORT );
  _ip = map->get( name + "_IP", "127.0.0.1");
  memset(m_pf_on_tges,0, sizeof(m_pf_on_tges));
}


void  PQLabsEvents::pollForInput(Array<EventRef> &events)
{
	if (_unpolledEvents.size() > 0)
	{
		_eventLock.lock();
		if (_unpolledEvents.size() > 0)
		{
			events.append(_unpolledEvents);
			_unpolledEvents.clear();
		}
		_eventLock.unlock();
	}
}



PQLabsEvents::~PQLabsEvents()
{
	cout << "Server disconnected." << endl;
	DisconnectServer();
}

/////////////////////////// functions ///////////////////////////////////
int PQLabsEvents::init()
{
	int err_code = PQMTE_SUCCESS;

	// initialize the handle functions of gestures;
	InitFuncOnTG();
	// set the functions on server callback
	SetFuncsOnReceiveProc();
	// connect server
	cout << " connect to server..." << endl;
	if((err_code = ConnectServer()) != PQMTE_SUCCESS){
		cout << " connect server fail, socket error code:" << err_code << endl;
		return err_code;
	}
	// send request to server
	cout << " connect success, send request." << endl;
	TouchClientRequest tcq = {0};
	tcq.type = RQST_RAWDATA_ALL | RQST_GESTURE_ALL;
	if((err_code = SendRequest(tcq)) != PQMTE_SUCCESS){
		cout << " send request fail, error code:" << err_code << endl;
		return err_code;
	}
	////////////you can set the move_threshold when the tcq.type is RQST_RAWDATA_INSIDE;
	////send threshold
	//int move_threshold = 0;// 0 pixel, receuve all the touch points that touching in the windows area of this client;
	//if((err_code = SendThreshold(move_threshold)) != PQMTE_SUCCESS){
	//	cout << " send threadhold fail, error code:" << err_code << endl;
	//	return err_code;
	//}
	
	//////// you can set the resolution of the touch point(raw data) here;
	//// setrawdata_resolution
	//int maxX = 32768, maxY = 32768;
	//if((err_code= SetRawDataResolution(maxX, maxY)) != PQMTE_SUCCESS){
	//	cout << " set raw data resolution fail, error code:" << err_code << endl;
	//}
	////////////////////////
	//get server resolution
	if((err_code = GetServerResolution(OnGetServerResolution, this)) != PQMTE_SUCCESS){
		cout << " get server resolution fail,error code:" << err_code << endl;
		return err_code;
	}
	//
	// start receiving
	cout << " send request success, start recv." << endl;
	return err_code;
}

void PQLabsEvents::InitFuncOnTG()
{
	// initialize the call back functions of toucha gestures;
	m_pf_on_tges[TG_TOUCH_START] = &PQLabsEvents::OnTG_TouchStart;
	m_pf_on_tges[TG_DOWN] = &PQLabsEvents::OnTG_Down;
	m_pf_on_tges[TG_MOVE] = &PQLabsEvents::OnTG_Move;
	m_pf_on_tges[TG_UP] = &PQLabsEvents::OnTG_Up;

	m_pf_on_tges[TG_SECOND_DOWN] = &PQLabsEvents::OnTG_SecondDown;
	m_pf_on_tges[TG_SECOND_UP] = &PQLabsEvents::OnTG_SecondUp;

	m_pf_on_tges[TG_SPLIT_START] = &PQLabsEvents::OnTG_SplitStart;
	m_pf_on_tges[TG_SPLIT_APART] = &PQLabsEvents::OnTG_SplitApart;
	m_pf_on_tges[TG_SPLIT_CLOSE] = &PQLabsEvents::OnTG_SplitClose;
	m_pf_on_tges[TG_SPLIT_END] = &PQLabsEvents::OnTG_SplitEnd;

	m_pf_on_tges[TG_TOUCH_END] = &PQLabsEvents::OnTG_TouchEnd;
}
void PQLabsEvents::SetFuncsOnReceiveProc()
{
	PFuncOnReceivePointFrame old_rf_func = SetOnReceivePointFrame(&PQLabsEvents::OnReceivePointFrame,this);
	PFuncOnReceiveGesture old_rg_func = SetOnReceiveGesture(&PQLabsEvents::OnReceiveGesture,this);
	PFuncOnServerBreak old_svr_break = SetOnServerBreak(&PQLabsEvents::OnServerBreak,NULL);
	PFuncOnReceiveError old_rcv_err_func = SetOnReceiveError(&PQLabsEvents::OnReceiveError,NULL);
	PFuncOnGetDeviceInfo old_gdi_func = SetOnGetDeviceInfo(&PQLabsEvents::OnGetDeviceInfo,NULL);
}

void PQLabsEvents::OnReceivePointFrame(int frame_id, int time_stamp, int moving_point_count, const TouchPoint * moving_point_array, void * call_back_object)
{
	PQLabsEvents * sample = static_cast<PQLabsEvents*>(call_back_object);
	//assert(PQLabsEvents != NULL);
	const char * tp_event[] = 
	{
		"down",
		"move",
		"up",
	};
	
	// cout << " frame_id:" << frame_id << " time:"  << time_stamp << " ms" << " moving point count:" << moving_point_count << endl;
	for(int i = 0; i < moving_point_count; ++ i){
		TouchPoint tp = moving_point_array[i];
		sample->OnTouchPoint(tp);
	}
	//throw exception("test exception here");
}
void PQLabsEvents:: OnReceiveGesture(const TouchGesture & ges, void * call_back_object)
{
	PQLabsEvents * sample = static_cast<PQLabsEvents*>(call_back_object);
	//assert(PQLabsEvents != NULL);
	sample->OnTouchGesture(ges);
	//throw exception("test exception here");
}
void PQLabsEvents:: OnServerBreak(void * param, void * call_back_object)
{
	// when the server break, disconenct server;
	cout << "server break, disconnect here" << endl;
	DisconnectServer();
}
void PQLabsEvents::OnReceiveError(int err_code, void * call_back_object)
{
	switch(err_code)
	{
	case PQMTE_RCV_INVALIDATE_DATA:
		cout << " error: receive invalidate data." << endl;
		break;
	case PQMTE_SERVER_VERSION_OLD:
		cout << " error: the multi-touch server is old for this client, please update the multi-touch server." << endl;
		break;
	case PQMTE_EXCEPTION_FROM_CALLBACKFUNCTION:
		cout << "**** some exceptions thrown from the call back functions." << endl;
		assert(0); //need to add try/catch in the callback functions to fix the bug;
		break;
	default:
		cout << " socket error, socket error code:" << err_code << endl;
	}
}
void PQLabsEvents:: OnGetServerResolution(int x, int y, void * call_back_object)
{
	PQLabsEvents * sample = static_cast<PQLabsEvents*>(call_back_object);
	//assert(PQLabsEvents != NULL);
	cout << " server resolution:" << x << "," << y << endl;
	sample->_width = x;
	sample->_height = y;
}
void PQLabsEvents::OnGetDeviceInfo(const TouchDeviceInfo & deviceinfo,void *call_back_object)
{
	cout << " touch screen, SerialNumber: " << deviceinfo.serial_number <<",(" << deviceinfo.screen_width << "," << deviceinfo.screen_height << ")."<<  endl;
}
// here, just record the position of point,
//	you can do mouse map like "OnTG_Down" etc;
void PQLabsEvents:: OnTouchPoint(const TouchPoint & tp)
{
	/*switch(tp.point_event)
	{
	case TP_DOWN:
		cout << "  point " << tp.id << " come at (" << tp.x << "," << tp.y 
			<< ") width:" << tp.dx << " height:" << tp.dy << endl;
		break;
	case TP_MOVE:
		cout << "  point " << tp.id << " move at (" << tp.x << "," << tp.y 
			<< ") width:" << tp.dx << " height:" << tp.dy << endl;
		break;
	case TP_UP:
		cout << "  point " << tp.id << " leave at (" << tp.x << "," << tp.y 
			<< ") width:" << tp.dx << " height:" << tp.dy << endl;
		break;
	}*/

	Vector2 pos = Vector2((float)tp.x/(float)_width, (float)tp.y/(float)_height);
	if(tp.point_event == TP_DOWN) {
		AddUnpolledEvent(new Event("TUIO_Cursor" + intToString(tp.id) + "_down", pos));
	}
	else if(tp.point_event == TP_MOVE) {
		AddUnpolledEvent(new Event("TUIO_CursorMove" + intToString(tp.id), pos));
	}
	else if(tp.point_event == TP_UP) {
		AddUnpolledEvent(new Event("TUIO_Cursor" + intToString(tp.id) + "_up"));
	}
}

void PQLabsEvents::AddUnpolledEvent(VRG3D::Event* value)
{
	_eventLock.lock();
	EventRef ref = value;
	_unpolledEvents.append(ref);
	_eventLock.unlock();
}

void PQLabsEvents:: OnTouchGesture(const TouchGesture & tg)
{
	if(TG_NO_ACTION == tg.type)
		return ;
	
	assert(tg.type <= TG_TOUCH_END);
	DefaultOnTG(tg,this);
	PFuncOnTouchGesture pf = m_pf_on_tges[tg.type];
	if(NULL != pf){
		pf(tg,this);
	}
}
void PQLabsEvents:: OnTG_TouchStart(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_TOUCH_START);
	cout << "  here, the touch start, initialize something." << endl;*/
}
void PQLabsEvents:: DefaultOnTG(const TouchGesture & tg,void * call_object) // just show the gesture
{
	/*cout <<"ges,name:"<< GetGestureName(tg) << " type:" << tg.type << ",param size:" << tg.param_size << " ";
	for(int i = 0; i < tg.param_size; ++ i)
		cout << tg.params[i] << " ";
	cout << endl;*/
}
void PQLabsEvents:: OnTG_Down(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_DOWN && tg.param_size >= 2);
	cout << "  the single finger touching at :( " 
		<< tg.params[0] << "," << tg.params[1] << " )" << endl;*/
}
void PQLabsEvents:: OnTG_Move(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_MOVE && tg.param_size >= 2);
	cout << "  the single finger moving on the screen at :( " 
		<< tg.params[0] << "," << tg.params[1] << " )" << endl;*/
}
void PQLabsEvents:: OnTG_Up(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_UP && tg.param_size >= 2);
	cout << " the single finger is leaving the screen at :( " 
		<< tg.params[0] << "," << tg.params[1] << " )" << endl;*/
}
//
void PQLabsEvents:: OnTG_SecondDown(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_SECOND_DOWN && tg.param_size >= 4);
	cout << "  the second finger touching at :( " 
		<< tg.params[0] << "," << tg.params[1] << " ),"
		<< " after the first finger touched at :( "
		<< tg.params[2] << "," << tg.params[3] << " )" << endl;*/
}
void PQLabsEvents:: OnTG_SecondUp(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_SECOND_UP && tg.param_size >= 4);
	cout << "  the second finger is leaving at :( " 
		<< tg.params[0] << "," << tg.params[1] << " ),"
		<< " while the first finger still anchored around :( "
		<< tg.params[2] << "," << tg.params[3] << " )" << endl;*/
}
//
void PQLabsEvents:: OnTG_SplitStart(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_SPLIT_START && tg.param_size >= 4);
	cout << "  the two fingers is splitting with one finger at: ( " 
		<< tg.params[0] << "," << tg.params[1] << " ),"
		<< " , the other at :( "
		<< tg.params[2] << "," << tg.params[3] << " )" << endl;*/
}

void PQLabsEvents:: OnTG_SplitApart(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_SPLIT_APART && tg.param_size >= 1);
	cout << "  the two fingers is splitting apart with there distance incresed by " 
		<< tg.params[0]
		<< " with a ratio :" << tg.params[1]
		<< endl;*/
}
void PQLabsEvents:: OnTG_SplitClose(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_SPLIT_CLOSE && tg.param_size >= 1);
	cout << "  the two fingers is splitting close with there distance decresed by " 
		<< tg.params[0]
		<< " with a ratio :" << tg.params[1]
		<< endl;*/
}
void PQLabsEvents:: OnTG_SplitEnd(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_SPLIT_END);
	cout << "  the two splitting fingers with one finger at: ( " 
		<< tg.params[0] << "," << tg.params[1] << " ),"
		<< " , the other at :( "
		<< tg.params[2] << "," << tg.params[3] << " )" 
		<< " will end" << endl;*/
}
// OnTG_TouchEnd: to clear what need to clear
void PQLabsEvents:: OnTG_TouchEnd(const TouchGesture & tg,void * call_object)
{
	/*assert(tg.type == TG_TOUCH_END);
	cout << "  all the fingers is leaving and there is no fingers on the screen." << endl;*/
}
/////////////////////////// functions ///////////////////////////////////

};         // end namespace VRG3D
