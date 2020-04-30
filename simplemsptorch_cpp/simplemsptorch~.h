/**
@file
simplemsptorch~.h: header for C++ version of simplemsptorch
created by: grigore burloiu, gburloiu@gmail.com
@ingroup examples
*/

#pragma once

#include "torch\torch.h"
#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects


class SimpleMspTorch;

extern "C" {
	// struct to represent the object's state
	typedef struct _simplemsptorch {
		t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
		SimpleMspTorch*	smt;

		void* out_sig;	// outlets must be defined
	} t_simplemsptorch;

	// method prototypes
	void *simplemsptorch_new(t_symbol *s, long argc, t_atom *argv);
	void simplemsptorch_free(t_simplemsptorch *x);
	void simplemsptorch_assist(t_simplemsptorch *x, void *b, long m, long a, char *s);
	void simplemsptorch_float(t_simplemsptorch *x, double f);
	void simplemsptorch_dsp64(t_simplemsptorch *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
	void simplemsptorch_perform64(t_simplemsptorch *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

	// global class pointer variable
	static t_class *simplemsptorch_class = NULL;
} // end extern "C"

class SimpleMspTorch {
public:
	t_simplemsptorch *max;

	// internal vars:
	float offset;
	torch::Tensor tensor;

	// constructor & destructor:
	SimpleMspTorch();
	~SimpleMspTorch();

	// DSP:
	void perform(double *in, double *out, long sampleframes);

	// inlet methods:
	void in_float(double f);
};