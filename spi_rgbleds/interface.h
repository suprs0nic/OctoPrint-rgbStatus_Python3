#pragma once

#ifndef INTERFACE_H
#define INTERFACE_H 1


#include <Python.h>
#include "RgbLightHandler.h"
#include "RgbLightConstant.h"
#include "RgbLightPulsing.h"
#include "RgbLightOnOff.h"

static RgbLightHandler * handler;

PyObject * initialize(PyObject *self, PyObject *args, PyObject *kwargs, char *keywords[]);
PyObject * start(PyObject *self, PyObject *args);
PyObject * stop(PyObject *self, PyObject *args);
PyObject * set_constant_color(PyObject *self, PyObject *args, PyObject *kwargs, char *keywords[]);
PyObject * set_pulsing_color(PyObject *self, PyObject *args, PyObject *kwargs, char *keywords[]);
PyObject * set_onoff_color(PyObject *self, PyObject *args, PyObject *kwargs, char *keywords[]);

extern "C" void initspirgbleds(void);

static PyMethodDef SpiRgbLedsMethods[] = {
	{ "initialize", (PyCFunction)initialize, METH_VARARGS | METH_KEYWORDS, "Settings of the RGB led driver" },
	{ "start", (PyCFunction)start, METH_VARARGS, "Start the RGB led driver" },
	{ "stop", (PyCFunction)stop, METH_VARARGS, "Stop the RGB led driver" },
	{ "set_constant_color", (PyCFunction)set_constant_color, METH_VARARGS | METH_KEYWORDS, "Set a constant color" },
	{ "set_pulsing_color", (PyCFunction)set_pulsing_color, METH_VARARGS | METH_KEYWORDS, "Set a pulsating color" },
	{ NULL, NULL, 0, NULL }        /* Sentinel */
};

void set_pattern(RgbLightPattern * pattern, char target);

#endif // !INTERFACE_H

