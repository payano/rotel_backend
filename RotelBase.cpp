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


#include <string.h>
#include "RotelBase.h"

namespace rotel {

RotelBase::RotelBase() {
	connected = false;
	int ret;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sock) {
		perror("socket");
		return;
	}

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(portnr);
	sock_addr.sin_addr.s_addr = inet_addr("10.10.20.124");
	addr_size = sizeof(sock_other);

	ret = connect(sock, (struct sockaddr*)&sock_addr, addr_size);
	if(ret) {
		perror("connect");
		return;
	}


	char *hello = "model!";
	char buffer[1024];
	send(sock, hello, strlen(hello), 0);
	printf("Hello message sent\n");
	int valread = read(sock, buffer, 1024);
	printf("%s\n", buffer);
	connected = true;
}

RotelBase::~RotelBase() {
	connected = false;
	close(sock);
}

std::string RotelBase::powerAndVolumeCommand(enum POWER_AND_VOLUME_COMMANDS cmd, int val) {
	switch(cmd) {
	case POWER_AND_VOLUME_COMMANDS::POWER_ON:     return "power_on!";
	case POWER_AND_VOLUME_COMMANDS::POWER_OFF:    return "power_off!";
	case POWER_AND_VOLUME_COMMANDS::POWER_TOGGLE: return "power_toggle!";
	case POWER_AND_VOLUME_COMMANDS::VOL_UP:       return "vol_up!";
	case POWER_AND_VOLUME_COMMANDS::VOL_DOWN:     return "vol_down!";
	case POWER_AND_VOLUME_COMMANDS::VOL_NN:       return "vol_" + val;
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