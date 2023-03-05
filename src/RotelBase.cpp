/*
 * rotelbase.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: johan
 */

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include "RotelA14.h"


#include <string>
#include "RotelBase.h"

#include <iostream>
#include <jni.h>

namespace rotel {

static constexpr int BUFFER_SZ = 512;
static constexpr int PORT_NO = 9590;
RotelBase::RotelBase() {
	connectRotel();
}

RotelBase::~RotelBase() {
	disconnectRotel();
}

void RotelBase::connectRotel() {
	connected = false;
	int ret;


	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sock) {
		perror("socket");
		return;
	}

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(PORT_NO);
	sock_addr.sin_addr.s_addr = inet_addr("10.10.20.124");
	addr_size = sizeof(sock_other);

	ret = connect(sock, (struct sockaddr*)&sock_addr, addr_size);
	if(ret) {
		perror("connect");
		return;
	}
	connected = true;
}

void RotelBase::disconnectRotel() {
	connected = false;
	shutdown(sock, SHUT_RDWR);
	close(sock);
}

void RotelBase::retrieveSettings() {
	if(!connected) connectRotel();
	if(!connected) {
		std::cout << "connection failed!" << std::endl;
		return;
	}

	for(auto &setting : features[COMMAND_TYPE::REQUEST_COMMANDS]) {
		auto cmd = static_cast<REQUEST_COMMANDS>(setting);
		std::string command = requestCommand(cmd);
		std::string recv = sendRecv(command);
		settings[cmd] = getValue(recv);
	}
}

const std::map<COMMAND_TYPE, std::vector<int>>& RotelBase::getFeatures() {
	return features;
}

void RotelBase::setFeature(COMMAND_TYPE cmd, int type, int val) {
	switch(cmd) {
	case COMMAND_TYPE::POWER_AND_VOLUME_COMMANDS: {
		auto command = static_cast<POWER_AND_VOLUME_COMMANDS>(type);
		std::string str_command = powerAndVolumeCommand(command, val);
		std::string recv = sendRecv(str_command);
		std::string recv_val = getValue(recv);
		switch(command) {
		case POWER_AND_VOLUME_COMMANDS::POWER_ON: /* fall through */
		case POWER_AND_VOLUME_COMMANDS::POWER_OFF: /* fall through */
		case POWER_AND_VOLUME_COMMANDS::POWER_TOGGLE:
			settings[REQUEST_COMMANDS::POWER] = recv_val;
			break;
		case POWER_AND_VOLUME_COMMANDS::VOL_UP: /* fall through */
		case POWER_AND_VOLUME_COMMANDS::VOL_DOWN: /* fall through */
		case POWER_AND_VOLUME_COMMANDS::VOL_NN:
			settings[REQUEST_COMMANDS::VOLUME] = recv_val;
			break;
		case POWER_AND_VOLUME_COMMANDS::MUTE: /* fall through */
		case POWER_AND_VOLUME_COMMANDS::MUTE_ON: /* fall through */
		case POWER_AND_VOLUME_COMMANDS::MUTE_OFF:
			settings[REQUEST_COMMANDS::MUTE] = recv_val;
			break;
		}
		break;
	}
	case COMMAND_TYPE::SOURCE_SELECTION_COMMANDS:{
		auto command = static_cast<SOURCE_SELECTION_COMMANDS>(type);
		std::string str_command = sourceSelectionCommand(command);
		std::string recv = sendRecv(str_command);
		std::string recv_val = getValue(recv);
		settings[REQUEST_COMMANDS::SOURCE] = recv_val;

		break;
	}
	case COMMAND_TYPE::SOURCE_CONTROL_COMMANDS:{
		auto command = static_cast<SOURCE_CONTROL_COMMANDS>(type);
		std::string str_command = sourceControlCommand(command);
		sendRecv(str_command);
		/* Nothing is updated... */
		break;

	}
	case COMMAND_TYPE::TONE_CONTROL_COMMANDS:{
		auto command = static_cast<TONE_CONTROL_COMMANDS>(type);
		std::string str_command = toneControlCommand(command);
		std::string recv = sendRecv(str_command);
		std::string recv_val = getValue(recv);
		switch(command) {
		case TONE_CONTROL_COMMANDS::BYPASS_ON: /* fall through */
		case TONE_CONTROL_COMMANDS::BYPASS_OFF: /* fall through */
			settings[REQUEST_COMMANDS::BYPASS] = recv_val;
			break;
		case TONE_CONTROL_COMMANDS::BASS_UP: /* fall through */
		case TONE_CONTROL_COMMANDS::BASS_DOWN: /* fall through */
		case TONE_CONTROL_COMMANDS::BASS_PLUS10: /* fall through */
		case TONE_CONTROL_COMMANDS::BASS_MINUS10: /* fall through */
		case TONE_CONTROL_COMMANDS::BASS_ZERO: /* fall through */
			settings[REQUEST_COMMANDS::BASS] = recv_val;
			break;
		case TONE_CONTROL_COMMANDS::TREBLE_UP: /* fall through */
		case TONE_CONTROL_COMMANDS::TREBLE_DOWN: /* fall through */
		case TONE_CONTROL_COMMANDS::TREBLE_PLUS10: /* fall through */
		case TONE_CONTROL_COMMANDS::TREBLE_MINUS10: /* fall through */
		case TONE_CONTROL_COMMANDS::TREBLE_ZERO: /* fall through */
			settings[REQUEST_COMMANDS::TREBLE] = recv_val;
			break;
		}
		break;

	}
	case COMMAND_TYPE::BALANCE_CONTROL_COMMANDS:{
		auto command = static_cast<BALANCE_CONTROL_COMMANDS>(type);
		std::string str_command = balanceControlCommand(command);
		std::string recv = sendRecv(str_command);
		std::string recv_val = getValue(recv);
		settings[REQUEST_COMMANDS::BALANCE] = recv_val;
		break;

	}
	case COMMAND_TYPE::SPEAKER_OUTPUT_COMMANDS:{
		auto command = static_cast<SPEAKER_OUTPUT_COMMANDS>(type);
		std::string str_command = speakerOutputCommand(command);
		std::string recv = sendRecv(str_command);
		std::string recv_val = getValue(recv);
		settings[REQUEST_COMMANDS::SPEAKER] = recv_val;
		break;

	}
	case COMMAND_TYPE::OTHER_COMMANDS:{
		auto command = static_cast<OTHER_COMMANDS>(type);
		std::string str_command = otherCommand(command);
		std::string recv = sendRecv(str_command);
		std::string recv_val = getValue(recv);
		settings[REQUEST_COMMANDS::DIMMER] = recv_val;
		break;

	}
	default: {
	}
	}
}

std::string RotelBase::sendRecv(std::string& msg) {
	char buffer[BUFFER_SZ] = {0};
	send(sock, msg.c_str(), msg.length(), 0);
	read(sock, buffer, sizeof(buffer));
	return buffer;
}

std::string RotelBase::getValue(std::string& val) {
	int first = val.find('=') + 1;
	int last = val.find('$');
	return val.substr(first, last - first);
}

std::unique_ptr<RotelBase> RotelBase::get(std::string ipv4_address){
	std::unique_ptr<RotelBase> ret;
	switch(getModel(ipv4_address)) {
	case SUPPORTED_MODELS::A14: ret = std::make_unique<RotelA14>(); break;
	default: return nullptr;
	}
	ret->retrieveSettings();
	return ret;
}

const std::map<REQUEST_COMMANDS, std::string>& RotelBase::getSettings() {
	return settings;
}

SUPPORTED_MODELS RotelBase::getModel(std::string &ipv4_address) {
	int ret;
	struct sockaddr_in sock_addr;
	struct sockaddr_in sock_other;
	int addr_size;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sock) {
		perror("socket");
		return SUPPORTED_MODELS::UNKNOWN;
	}

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(PORT_NO);
	sock_addr.sin_addr.s_addr = inet_addr(ipv4_address.c_str());
	addr_size = sizeof(sock_other);

	ret = connect(sock, (struct sockaddr*)&sock_addr, addr_size);
	if(ret) {
		perror("connect");
		close(sock);
		return SUPPORTED_MODELS::UNKNOWN;
	}

	std::string model = requestCommand(REQUEST_COMMANDS::MODEL);
	char buffer[BUFFER_SZ] = {0};
	send(sock, model.c_str(), model.length(), 0);
	read(sock, buffer, sizeof(buffer));
	shutdown(sock, SHUT_RDWR);
	close(sock);
	return getSupportedModel(buffer);

}

SUPPORTED_MODELS RotelBase::getSupportedModel(char* model) {
	if(0 == strcmp(model, "model=a14$")) return SUPPORTED_MODELS::A14;

	return SUPPORTED_MODELS::UNKNOWN;
}


std::string RotelBase::powerAndVolumeCommand(enum POWER_AND_VOLUME_COMMANDS cmd, int val) {
	switch(cmd) {
	case POWER_AND_VOLUME_COMMANDS::POWER_ON:     return "power_on!";
	case POWER_AND_VOLUME_COMMANDS::POWER_OFF:    return "power_off!";
	case POWER_AND_VOLUME_COMMANDS::POWER_TOGGLE: return "power_toggle!";
	case POWER_AND_VOLUME_COMMANDS::VOL_UP:       return "vol_up!";
	case POWER_AND_VOLUME_COMMANDS::VOL_DOWN:     return "vol_down!";
	case POWER_AND_VOLUME_COMMANDS::VOL_NN:       return "vol_" + std::to_string(val);
	case POWER_AND_VOLUME_COMMANDS::MUTE:         return "mute!";
	case POWER_AND_VOLUME_COMMANDS::MUTE_ON:      return "mute_on!";
	case POWER_AND_VOLUME_COMMANDS::MUTE_OFF:     return "mute_off!";
	default: return "";
	}
}

std::string RotelBase::sourceSelectionCommand(enum SOURCE_SELECTION_COMMANDS cmd) {
	switch(cmd) {
	case SOURCE_SELECTION_COMMANDS::CD:        return "cd!";
	case SOURCE_SELECTION_COMMANDS::COAX1:     return "coax1!";
	case SOURCE_SELECTION_COMMANDS::COAX2:     return "coax2!";
	case SOURCE_SELECTION_COMMANDS::OPT1:      return "opt1!";
	case SOURCE_SELECTION_COMMANDS::OPT2:      return "opt2!";
	case SOURCE_SELECTION_COMMANDS::AUX1:      return "aux1!";
	case SOURCE_SELECTION_COMMANDS::AUX2:      return "aux2!";
	case SOURCE_SELECTION_COMMANDS::TUNER:     return "tuner!";
	case SOURCE_SELECTION_COMMANDS::PHONO:     return "phono!";
	case SOURCE_SELECTION_COMMANDS::USB:       return "usb!";
	case SOURCE_SELECTION_COMMANDS::BLUETOOTH: return "bluetooth!";
	case SOURCE_SELECTION_COMMANDS::PCUSB:     return "pcusb!";
	default: return "";
	}

}

std::string RotelBase::sourceControlCommand(SOURCE_CONTROL_COMMANDS cmd) {
	switch(cmd) {
	case SOURCE_CONTROL_COMMANDS::PLAY:  return "play!";
	case SOURCE_CONTROL_COMMANDS::STOP:  return "stop!";
	case SOURCE_CONTROL_COMMANDS::PAUSE: return "pause!";
	case SOURCE_CONTROL_COMMANDS::TRKF:  return "trkf!";
	case SOURCE_CONTROL_COMMANDS::TRKB:  return "trkb!";
	default: return "";
	}
}

std::string RotelBase::toneControlCommand(TONE_CONTROL_COMMANDS cmd) {
	switch(cmd) {
	case TONE_CONTROL_COMMANDS::BYPASS_ON:      return "bypass_on!";
	case TONE_CONTROL_COMMANDS::BYPASS_OFF:     return "bypass_off!";
	case TONE_CONTROL_COMMANDS::BASS_UP:        return "bass_up!";
	case TONE_CONTROL_COMMANDS::BASS_DOWN:      return "bass_down!";
	case TONE_CONTROL_COMMANDS::BASS_PLUS10:    return "bass_+10!";
	case TONE_CONTROL_COMMANDS::BASS_MINUS10:   return "bass_-10!";
	case TONE_CONTROL_COMMANDS::BASS_ZERO:      return "bass_000!";
	case TONE_CONTROL_COMMANDS::TREBLE_UP:      return "treble_up!";
	case TONE_CONTROL_COMMANDS::TREBLE_DOWN:    return "treble_down!";
	case TONE_CONTROL_COMMANDS::TREBLE_PLUS10:  return "treble_+10!";
	case TONE_CONTROL_COMMANDS::TREBLE_MINUS10: return "treble_-10!";
	case TONE_CONTROL_COMMANDS::TREBLE_ZERO:    return "treble_000!";
	default: return "";
	}
}

std::string RotelBase::balanceControlCommand(BALANCE_CONTROL_COMMANDS cmd) {
	switch(cmd) {
	case BALANCE_CONTROL_COMMANDS::BALANCE_R:    return "balance_r!";
	case BALANCE_CONTROL_COMMANDS::BALANCE_L:    return "balance_l!";
	case BALANCE_CONTROL_COMMANDS::BALANCE_L15:  return "balance_L15!";
	case BALANCE_CONTROL_COMMANDS::BALANCE_R15:  return "balance_R15!";
	case BALANCE_CONTROL_COMMANDS::BALANCE_ZERO: return "balance_000!";
	default: return "";
	}
}

std::string RotelBase::speakerOutputCommand(SPEAKER_OUTPUT_COMMANDS cmd) {
	switch(cmd) {
	case SPEAKER_OUTPUT_COMMANDS::SPEAKER_A:     return "speaker_a!";
	case SPEAKER_OUTPUT_COMMANDS::SPEAKER_B:     return "speaker_b!";
	case SPEAKER_OUTPUT_COMMANDS::SPEAKER_A_ON:  return "speaker_a_on!";
	case SPEAKER_OUTPUT_COMMANDS::SPEAKER_A_OFF: return "speaker_a_off!";
	case SPEAKER_OUTPUT_COMMANDS::SPEAKER_B_ON:  return "speaker_b_on!";
	case SPEAKER_OUTPUT_COMMANDS::SPEAKER_B_OFF: return "speaker_b_off!";
	default: return "";
	}
}

std::string RotelBase::otherCommand(OTHER_COMMANDS cmd) {
	switch(cmd) {
	case OTHER_COMMANDS::DIMMER:   return "dimmer!";
	case OTHER_COMMANDS::DIMMER_0: return "dimmer_0!";
	case OTHER_COMMANDS::DIMMER_1: return "dimmer_1!";
	case OTHER_COMMANDS::DIMMER_2: return "dimmer_2!";
	case OTHER_COMMANDS::DIMMER_3: return "dimmer_3!";
	case OTHER_COMMANDS::DIMMER_4: return "dimmer_4!";
	case OTHER_COMMANDS::DIMMER_5: return "dimmer_5!";
	case OTHER_COMMANDS::DIMMER_6: return "dimmer_6!";
	default: return "";
	}
}

std::string RotelBase::requestCommand(REQUEST_COMMANDS cmd) {
	switch(cmd) {
	case REQUEST_COMMANDS::POWER:      return "power?";
	case REQUEST_COMMANDS::SOURCE:     return "source?";
	case REQUEST_COMMANDS::VOLUME:     return "volume?";
	case REQUEST_COMMANDS::MUTE:       return "mute?";
	case REQUEST_COMMANDS::BYPASS:     return "bypass?";
	case REQUEST_COMMANDS::BASS:       return "bass?";
	case REQUEST_COMMANDS::TREBLE:     return "treble?";
	case REQUEST_COMMANDS::BALANCE:    return "balance?";
	case REQUEST_COMMANDS::FREQ:       return "freq?";
	case REQUEST_COMMANDS::SPEAKER:    return "speaker?";
	case REQUEST_COMMANDS::DIMMER:     return "dimmer?";
	case REQUEST_COMMANDS::PCUSB:      return "pcusb?";
	case REQUEST_COMMANDS::VERSION:    return "version?";
	case REQUEST_COMMANDS::PC_VERSION: return "pc_version?";
	case REQUEST_COMMANDS::IP:         return "ip?";
	case REQUEST_COMMANDS::MAC:        return "mac?";
	case REQUEST_COMMANDS::MODEL:      return "model?";
	case REQUEST_COMMANDS::DISCOVER:   return "discover?";
	default: return "";
	}
}

} /* namespace rotel */

extern "C"
JNIEXPORT void JNICALL
Java_com_example_rotel_RotelLib_sayHello(JNIEnv *env, jobject thiz) {
    // TODO: implement sayHello()
    printf("LINE: %d\n", __LINE__);
    std::cout << "IS IT RIGHT??" << std::endl;

    static std::unique_ptr<rotel::RotelBase> r = rotel::RotelBase::get("10.10.20.124");
    //if(nullptr == r) return;
    const std::map<int, std::vector<int>> features = (const std::map<int, std::vector<int>> &) r->getFeatures();

    jclass mapClass = env->FindClass("java/util/HashMap");
    jclass cls = env->FindClass("java/lang/Integer");

    if(mapClass == NULL || cls == NULL)
        return;
/*
    jmethodID init = env->GetMethodID(mapClass, "<init>", "()V");
    jobject hashMap = env->NewObject(mapClass, init);
    jmethodID put = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
*/

    /*
     * // Grab Fields
        jclass cls = env->GetObjectClass(obj);
        jfieldID fid = env->GetFieldID(cls, "testField", "[I");

        jintArray jary;
        jary = (jintArray)env->GetObjectField(obj, fid);
        jint *body = env->GetIntArrayElements(jary, 0);
        body[0] = 3000;
        env->ReleaseIntArrayElements(jary, body, 0);
     */
    jsize map_len = 1;

    jmethodID init = env->GetMethodID(mapClass, "<init>", "(I)V");
    jobject hashMap = env->NewObject(mapClass, init, map_len);
    jmethodID midInit = env->GetMethodID(cls, "<init>", "(I)V");

    jmethodID put = env->GetMethodID(mapClass, "put",
                                        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    printf("LINE: %d\n", __LINE__);

    for(auto &key : features) {
        jobject keyValue = env->NewObject(cls, midInit, key.first);
        jintArray itemArray = env->NewIntArray(key.second.size());
        jint *body = env->GetIntArrayElements(itemArray, 0);
        for(size_t i = 0; i < key.second.size(); ++i) {
            body[i] = key.second[i];
        }
        env->CallObjectMethod(hashMap, put, keyValue, itemArray);
        env->DeleteLocalRef(keyValue);
        //env->ReleaseIntArrayElements(itemArray, body, 0);
    }
	(void)env;
	(void)thiz;
}

static std::unique_ptr<rotel::RotelBase> r;
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_rotel_RotelLib_cpp_1getFeatures(JNIEnv *env, jobject thiz) {
	(void)thiz;
		jclass mapClass = env->FindClass("java/util/HashMap");
		if(mapClass == NULL)
			return NULL;

	jclass cls = env->FindClass("java/lang/Integer");
	jmethodID midInit = env->GetMethodID(cls, "<init>", "(I)V");

	//if(nullptr == r) return;
	const std::map<int, std::vector<int>> map = (const std::map<int, std::vector<int>> &) r->getFeatures();

	jclass java_util_ArrayList      = static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
	jmethodID java_util_ArrayList_     = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
	//jmethodID java_util_ArrayList_size = env->GetMethodID (java_util_ArrayList, "size", "()I");
	//jmethodID java_util_ArrayList_get  = env->GetMethodID(java_util_ArrayList, "get", "(I)Ljava/lang/Object;");
	jmethodID java_util_ArrayList_add  = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");



	jmethodID init = env->GetMethodID(mapClass, "<init>", "()V");
		jobject hashMap = env->NewObject(mapClass, init);
		jmethodID put = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

		std::map<int, std::vector<int>>::const_iterator citr = map.begin();
		int i = 0;
		for( ; citr != map.end(); ++citr) {
			jobject keyJava = env->NewObject(cls,midInit, i++);
			//jobject valueJava = env->NewObject(cls,midInit, 3);

			jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_, (int)citr->second.size());
			for(auto& item : citr->second) {
				jobject value = env->NewObject(cls, midInit, (int)item);
				env->CallBooleanMethod(result, java_util_ArrayList_add, value);
				env->DeleteLocalRef(value);
			}

			env->CallObjectMethod(hashMap, put, keyJava, result);

			env->DeleteLocalRef(keyJava);
			env->DeleteLocalRef(result);
		}

		jobject hashMapGobal = static_cast<jobject>(env->NewGlobalRef(hashMap));
		env->DeleteLocalRef(hashMap);
		env->DeleteLocalRef(mapClass);

		return hashMapGobal;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_rotel_RotelLib_cpp_1init(JNIEnv *env, jobject thiz) {
	(void) env;
	(void) thiz;
	r = rotel::RotelBase::get("10.10.20.124");
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_rotel_RotelLib_cpp_1deinit(JNIEnv *env, jobject thiz) {
	(void) env;
	(void) thiz;
	r = nullptr;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_rotel_RotelLib_cpp_1getSettings(JNIEnv *env, jobject thiz) {
	(void) env;
	(void) thiz;
	r->getSettings();
	return nullptr;
}
