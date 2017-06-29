/**
 * sem_checker.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This file contains the semantic analyzer for the AST of Yoakke.
 */

#pragma once

#include "sym_tab.h"

struct ty_symbol;

/**
 * A semantic checker contains symbolic information and inserts them
 * based on the encountered nodes.
 */
struct sem_checker {
public:


private:
	sym_tab<ty_symbol> m_Types;
};
