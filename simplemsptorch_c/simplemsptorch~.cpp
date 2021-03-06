/**
	@file
	simplemsptorch~: a simple audio object for Max, w/ LibTorch
	original by: jeremy bernstein, jeremy@bootsquad.com
	created by: grigore burloiu, gburloiu@gmail.com
	@ingroup examples
*/
#include "torch\torch.h"
#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects


// struct to represent the object's state
typedef struct _simplemsptorch {
	t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
	double			offset; 	// the value of a property of our object
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


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("simplemsptorch~", (method)simplemsptorch_new, (method)dsp_free, (long)sizeof(t_simplemsptorch), 0L, A_GIMME, 0);

	class_addmethod(c, (method)simplemsptorch_float,		"float",	A_FLOAT, 0);
	class_addmethod(c, (method)simplemsptorch_dsp64,		"dsp64",	A_CANT, 0);
	class_addmethod(c, (method)simplemsptorch_assist,	"assist",	A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	simplemsptorch_class = c;
}


void *simplemsptorch_new(t_symbol *s, long argc, t_atom *argv)
{
	t_simplemsptorch *x = (t_simplemsptorch *)object_alloc(simplemsptorch_class);

	if (x) {
		dsp_setup((t_pxobject *)x, 1);	// MSP inlets: arg is # of inlets and is REQUIRED!
		// use 0 if you don't need inlets
		outlet_new(x, "signal"); 		// signal outlet (note "signal" rather than NULL)
		x->offset = 2.0;

		torch::Tensor tensor = torch::rand({ 2, 3 });
		// Max uses printf to post, we can't use iostream...
		post("random tensor: %.2f %.2f %.2f | %.2f %.2f %.2f ", 
			tensor[0][0].item<float>(), tensor[0][1].item<float>(), tensor[0][2].item<float>(),
			tensor[1][0].item<float>(), tensor[1][1].item<float>(), tensor[1][2].item<float>());
	}
	return (x);
}


// NOT CALLED!, we use dsp_free for a generic free function
void simplemsptorch_free(t_simplemsptorch *x)
{
	;
}


void simplemsptorch_assist(t_simplemsptorch *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}


void simplemsptorch_float(t_simplemsptorch *x, double f)
{
	x->offset = f;
}


// registers a function for the signal chain in Max
void simplemsptorch_dsp64(t_simplemsptorch *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	post("my sample rate is: %f", samplerate);

	// instead of calling dsp_add(), we send the "dsp_add64" message to the object representing the dsp chain
	// the arguments passed are:
	// 1: the dsp64 object passed-in by the calling function
	// 2: the symbol of the "dsp_add64" message we are sending
	// 3: a pointer to your object
	// 4: a pointer to your 64-bit perform method
	// 5: flags to alter how the signal chain handles your object -- just pass 0
	// 6: a generic pointer that you can use to pass any additional data to your perform method

	object_method(dsp64, gensym("dsp_add64"), x, simplemsptorch_perform64, 0, NULL);
}


// this is the 64-bit perform method audio vectors
void simplemsptorch_perform64(t_simplemsptorch *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *inL = ins[0];		// we get audio for each inlet of the object from the **ins argument
	t_double *outL = outs[0];	// we get audio for each outlet of the object from the **outs argument
	int n = sampleframes;

	// this perform method simply copies the input to the output, offsetting the value
	while (n--)
		*outL++ = *inL++ + x->offset;
}

