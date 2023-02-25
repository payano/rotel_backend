#pragma once

#include <string>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include <memory>
namespace rotel {

//power_on! Power On power=on$
//power_off! Power Off power=standby$
//power_toggle! Power Toggle power=on/standby$
//vol_up! Volume Up volume=##$
//vol_dwn! Volume Down volume=##$
//vol_nn! Set Volume to level n (01 - 96) volume=##$
//mute! Mute Toggle mute=on/off$
//mute_on! Mute On mute=on$
//mute_off! Mute Off mute=off$
enum class POWER_AND_VOLUME_COMMANDS {
	POWER_ON = 0,
	POWER_OFF,
	POWER_TOGGLE,
	VOL_UP,
	VOL_DOWN,
	VOL_NN,
	MUTE,
	MUTE_ON,
	MUTE_OFF,
};

//cd! Source CD source=cd$
//coax1! Source Coax 1 source=coax1$
//coax2! Source Coax 2 source=coax2$
//opt1! Source Optical 1 source=opt1$
//opt2! Source Optical 2 source=opt2$
//aux1! Source Aux 1 source=aux1$
//aux2! Source Aux 2 source=aux2$
//tuner! Source Tuner source=tuner$
//phono! Source Phono source=phono$
//usb! Source Front USB source=usb$
//bluetooth! Source Bluetooth source=bluetooth$
//pcusb! Source PC-USB source=pc_usb$
enum class SOURCE_SELECTION_COMMANDS {
	CD = 0,
	COAX1,
	COAX2,
	OPT1,
	OPT2,
	AUX1,
	AUX2,
	TUNER,
	PHONO,
	USB,
	BLUETOOTH,
	PCUSB
};

//play! Play Source n/a
//stop! Stop Source n/a
//pause! Pause Source n/a
//trkf! Track Forward/Tune Up n/a
//trkb! Track Backward/Tune Down n/a

enum class SOURCE_CONTROL_COMMANDS {
	PLAY = 0,
	STOP,
	PAUSE,
	TRKF,
	TRKB
};

//bypass_on! Tone Bypass On bypass=on$
//bypass_off! Tone Bypass Off bypass=off$
//bass_up! Bass Up bass=000/+##/-##$
//bass_down! Bass Down bass=000/+##/-##$
//bass_-10! Set Bass to -10 bass=-10$
//bass_000! Set Bass to 0 bass=000$
//bass_+10! Set Bass to +10 bass=+10$
//treble_up! Treble Up treble=000/+##/-##$
//treble_down! Treble Down treble=000/+##/-##$
//treble_-10! Set Treble to -10 treble=-10$
//treble_000! Set Treble to 0 treble=000$
//treble_+10! Set Treble to +10 treble=+10$
//BALANCE CONTROL COMMAN

enum class TONE_CONTROL_COMMANDS {
	BYPASS_ON = 0,
	BYPASS_OFF,
	BASS_UP,
	BASS_DOWN,
	BASS_PLUS10,
	BASS_MINUS10,
	BASS_ZERO,
	TREBLE_UP,
	TREBLE_DOWN,
	TREBLE_PLUS10,
	TREBLE_MINUS10,
	TREBLE_ZERO
};

//BALANCE CONTROL COMMANDS
//balance_r! Balance Right balance=000/L##/R##$
//balance_l! Balance Left balance=000/L##/R##$
//balance_L15! Set Balance to Max Left balance=L15$
//balance_000! Set Balance to 0 balance=000$
//balance_R15! Set Balance to Max Right balance=R15$

enum class BALANCE_CONTROL_COMMANDS {
	BALANCE_R = 0,
	BALANCE_L,
	BALANCE_L15,
	BALANCE_R15,
	BALANCE_ZERO
};

//SPEAKER OUTPUT COMMANDS
//speaker_a! Toggle Speaker A Output speaker=a/a_b/off$
//speaker_b! Toggle Speaker B Output speaker=b/a_b/off$
//speaker_a_on! Speaker A Output On speaker=a/a_b$
//speaker_a_off! Speaker A Output Off speaker=b/off$
//speaker_b_on! Speaker B Output On speaker=b/a_b$
//speaker_b_off! Speakler B Output Off speaker=a/off$

enum class SPEAKER_OUTPUT_COMMANDS {
	SPEAKER_A = 0,
	SPEAKER_B,
	SPEAKER_A_ON,
	SPEAKER_A_OFF,
	SPEAKER_B_ON,
	SPEAKER_B_OFF
};

//OTHER COMMANDS
//dimmer! Toggle display dimmer dimmer=#$
//dimmer_0! Set display to brightest setting dimmer=0$
//dimmer_1! Set display to dimmer level 1 dimmer=1$
//dimmer_2! Set display to dimmer level 2 dimmer=2$
//dimmer_3! Set display to dimmer level 3 dimmer=3$
//dimmer_4! Set display to dimmer level 4 dimmer=4$
//dimmer_5! Set display to dimmer level 5 dimmer=5$
//dimmer_6! Set display to dimmest setting dimmer=6$

enum class OTHER_COMMANDS {
	DIMMER = 0,
	DIMMER_0,
	DIMMER_1,
	DIMMER_2,
	DIMMER_3,
	DIMMER_4,
	DIMMER_5,
	DIMMER_6,
};

enum class REQUEST_COMMANDS {
	POWER = 0,
	SOURCE,
	VOLUME,
	MUTE,
	BYPASS,
	BASS,
	TREBLE,
	BALANCE,
	FREQ,
	SPEAKER,
	DIMMER,
	PCUSB,
	VERSION,
	PC_VERSION,
	IP,
	MAC,
	MODEL,
	DISCOVER
};

enum class COMMAND_TYPE {
	POWER_AND_VOLUME_COMMANDS = 0,
	SOURCE_SELECTION_COMMANDS,
	SOURCE_CONTROL_COMMANDS,
	TONE_CONTROL_COMMANDS,
	BALANCE_CONTROL_COMMANDS,
	SPEAKER_OUTPUT_COMMANDS,
	OTHER_COMMANDS,
	REQUEST_COMMANDS
};

enum class SUPPORTED_MODELS {
	UNKNOWN = 0,
	A14
};

union commands {
	POWER_AND_VOLUME_COMMANDS pwr;
	SOURCE_SELECTION_COMMANDS src;
	SOURCE_CONTROL_COMMANDS   ctrl;
	TONE_CONTROL_COMMANDS     tone;
	BALANCE_CONTROL_COMMANDS  bal;
	SPEAKER_OUTPUT_COMMANDS   spkr;
	OTHER_COMMANDS            othr;
	REQUEST_COMMANDS          req;
};


struct COMMANDS {
	COMMAND_TYPE type;
	union commands command;
};

class RotelBase {
public:
	RotelBase();
	virtual ~RotelBase();

	virtual const std::map<COMMAND_TYPE, std::vector<int>>& getFeatures() = 0;
	void setFeature(COMMAND_TYPE, int);
	void getSettings();
	static std::unique_ptr<RotelBase> get(std::string);


protected:
	std::map<COMMAND_TYPE, std::vector<int>> features;
	std::map<REQUEST_COMMANDS, std::string> settings;

private:
	bool connected;
	void connectRotel();
	void disconnectRotel();
	std::string sendRecv(std::string);
	static SUPPORTED_MODELS getModel(std::string &);
	static SUPPORTED_MODELS getSupportedModel(char*);


	std::string powerAndVolumeCommand(enum POWER_AND_VOLUME_COMMANDS, int);
	std::string sourceSelectionCommand(enum SOURCE_SELECTION_COMMANDS);
	std::string sourceControlCommand(SOURCE_CONTROL_COMMANDS);
	std::string toneControlCommand(TONE_CONTROL_COMMANDS);
	std::string balanceControlCommand(BALANCE_CONTROL_COMMANDS);
	std::string speakerOutputCommand(SPEAKER_OUTPUT_COMMANDS);
	std::string otherCommand(OTHER_COMMANDS);
	std::string requestCommand(REQUEST_COMMANDS);

	struct sockaddr_in sock_addr;
	struct sockaddr_in sock_other;

	int addr_size;
	int sock;
};

} /* namespace rotel */

