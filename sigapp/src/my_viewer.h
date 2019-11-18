# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>
# include <sig/sn_lines.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	SnGroup* heli, * small_p, * big_p, * carrier;
	SnGroup* shadowHeli;
	SnTransform* body, * c, * p, * entire, * bprop, * baxis, * saxis, * sprop, * t, * sH,*h;
	SnLines* l;
	int altitude = 0;
	float speed_r = 0.0f, offset = 0.0;
	bool engine_on = false, mode = false;
	int object = 0;//0 = Heli, 1 = Carrier, 2 = Both only if landed. 
	float angle = gspi / 20;
	int cMode = 0;

	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;

	GsVec q = GsVec(0.0f,40.0f,0.0f);

	
   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui();
	void options(int c);
	void import_models();
	void align_models();
	void compute_mappings(char* file);
	void build_pad();
	void run_propellors(bool on);
	void translateY(SnTransform* t, float y);
	void translateX(SnTransform* t, float x);
	void translateZ(SnTransform* t, float z);
	void translate(SnTransform* t, float x, float y, float z);
	void rotateX(SnTransform* t, float r);
	void rotateY(SnTransform* t, float r);
	void rotateZ(SnTransform* t, float r);
	void scaleBody(SnTransform* t, float s);
	void cameraMode(int mode);
	void computeShadow();

	void run_animation ();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
	virtual void spin_animation_occured() override;
	virtual int handle(const GsEvent& e) override;

};

