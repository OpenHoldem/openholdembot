#pragma once
//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: A very simple chat system for OpenHoldem,
//	providing:
//	  - an interface for user.dll
//	  - (limited) chat support for the OH scripting language.
//
//  PokerChat is non-OOP, as we pass a function pointer to DLLs,
//
//******************************************************************************

#include "User/user.h"

//  Enumeration constants for simple chat messages.
//
enum
{
	no_simple_chat = 0,
	first_chat_message = 1,
 	simple_chat_gg = first_chat_message,
 	simple_chat_nh,
	simple_chat_vnh,
 	simple_chat_n1,
 	simple_chat_wow,
	simple_chat_lol,
 	simple_chat_rofl,
 	simple_chat_haha,
 	simple_chat_tu,
 	simple_chat_thx, 	
 	simple_chat_omg,
 	simple_chat_sick,
 	simple_chat_fu,
	simple_chat_fish,
 	simple_chat_hi,
 	simple_chat_hello,
 	simple_chat_bye,
	simple_chat_happy,  //  Message: ":)"
	simple_chat_sad,	  //  Message: ":("
	last_chat_message = simple_chat_sad
};

//  Chat enabled in preferences
//	and safety delay expired?
bool IsChatAllowed();

void ComputeFirstPossibleNextChatTime();

//  Simple messages for the formula guys.
//	Use f$chat as "the_MessageIndex".
void RegisterChatMessage(double message_index);

//  Pointer to the chatmessage to store it,
//	until it is processed and send to the keyboard
//
extern char *_the_chat_message;