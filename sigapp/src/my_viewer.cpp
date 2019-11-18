
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
# include <sig/sn_material.h>

# include <sigogl/ws_run.h>


/*

CAUTION!! This program uses the current SIG-11-13 because of the added SnMaterial

*/
//Speed of translations!
float speed = 0.8f;
//Global Models
SnModel* sa;
SnModel* ba;
SnModel* pad[8];
SnModel* model[4];
SnModel* harrier[7];
//Not used
//SnGroup* sun;
//SnModel* s;
MyViewer::MyViewer(int x, int y, int w, int h, const char* l) : WsViewer(x, y, w, h, l)
{
	_nbut = 0;
	_animating = false;
	build_ui();
	import_models();
	align_models();
	options(0);
	
}

void MyViewer::build_ui ()
{
	UiPanel* p;// *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	/*p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}*/
	p->add ( new UiButton ( "Animate", EvAnimate ) );
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();

	
	light().position.z = 2.0f;
	light().position.y = 2.0f; 
	//light().diffuse = GsColor::darkgray;

	//light().position.x = 1.0f;
	light().ambient = GsColor::gray;
	light().specular = GsColor::brown;
	
}
void MyViewer::options(int c) {
	switch (c) {

	case 0: {
		output("Hello there human:) \nPressing Keys:\n\
			'q' = Rotate Main Propellor Counter-clockwise\n\
			'a' = Rotate Main Propellor Clockwise\n\
			'w' = Rotate Tail Propellor Counter-clockwise\n\
			's' = Rotate Tail Propellor Clockwise\n\
			'z' = Scale Up\n\
			'x' = Scale Down\n\
			'SPACE' = Change Camera View\n\n\
			'ENTER' = Turn On Helicopter, must be on to control\n\
			'm' = Translate Up/Down or Move forward/backwards (Helicopter)\n\
			'c' = Helicopter or Carrier\n\
			'Up' = Translate Up or Move Forward\n\
			'Down' = Translate Down or Move Back\n\
			'Left' = Change Direction\n\
			'Right' = Change Direction");
		//output_pos(20, 70);
		break;
	}
	case 1: {
		output("Turn On Helictopter!");
		break;
	}
	case 2: {
		if(engine_on)
		output("Helicopter is now on!");
		else output("Helicopter is now off!");

		break;
	}
	case 4: {
		output("Cant go any lower becasause realism!");
		break;
	}
	case 5: {
		output("Changed Camera!");
		break;
	}
	case 6: {
		output("Moving!");
		break;
	}
	case 7: {
		output("Rotating!");
		break;
	}
	case 8: {
		output("Scaling!");
		break;
	}
	case 9: {
		output("Changed Mode!");
		break;
	}
	case 10: {
		output("Changed Object!");
		break;
	}
	}
	char* m,* o,* e;
	if (engine_on) e = "ON";
	else e = "OFF";

	if (object == 0) o = "HELICOPTER";
	else o = "CARRIER";
	if (!mode) m = "UP or DOWN";
	else m = "FORWARD or BACK";
	message().setf("Current Values:\t\
					Mode: %s\t\
					Object: %s\t\
					Engine: %s",m,o,e);
}

void MyViewer::import_models ()
{
	//Allocating Memory first.
	sa = new SnModel();
	ba = new SnModel();
	heli = new SnGroup;
	small_p = new SnGroup;
	big_p = new SnGroup;
	carrier = new SnGroup;
	shadowHeli = new SnGroup;

	for (int i = 0; i < 4; i++) {
		model[i] = new SnModel();
	}
	for (int i = 0; i < 8; i++) {
		pad[i] = new SnModel();
	}
	for (int i = 0; i < 7; i++) {
		harrier[i] = new SnModel();
	}
	//Building HelicopterPads and making sure its positioned correctly before any transformations
	build_pad();
	pad[0]->model()->translate(GsVec(0, 5.4f, 0));
	pad[1]->model()->translate(GsVec(10, 5.4f, 0));
	pad[2]->model()->translate(GsVec(-10, 5.4f, 0));
	pad[3]->model()->translate(GsVec(-20, 5.4f, 0));
	pad[4]->model()->translate(GsVec(-30, 5.4f, 0));
	pad[5]->model()->translate(GsVec(-40, 5.4f, 0));
	pad[6]->model()->translate(GsVec(20, 5.4f, 0));
	pad[7]->model()->translate(GsVec(30, 5.4f, 0));

	//Loading/creating the models from the .obj file included.
	model[0]->model()->load_obj("../model/heliBody.obj");
	model[1]->model()->load_obj("../model/heliProp.obj");
	model[2]->model()->load_obj("../model/heliProp2.obj");
	model[3]->model()->load_obj("../model/heliCarrier2.obj");


	harrier[0]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[1]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[2]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[3]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[4]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[5]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[6]->model()->load_obj("../model/Sea_Harrier.obj");

	//Making sure cylinders are perfectly at the origin, like every object.
	//This idea is so that the transformations are computed perfectly.
	sa->model()->make_cylinder(GsPnt(-0.5, 0.0, 0.0), GsPnt(0.5, 0.0, 0.0), 0.5f, 0.5f, 25, true);
	ba->model()->make_cylinder(GsPnt(-0.7, 0.0, 0.0), GsPnt(0.3, 0.0, 0.0), 0.4f, 0.3f, 25, true);
	sa->color(GsColor::red);
	ba->color(GsColor::red);

	model[0]->model()->translate(GsVec(0, 0, 0));
	model[1]->model()->translate(GsVec(2.5f, 0, -0.1f));
	model[2]->model()->translate(GsVec(0.6f, 0.0f, -0.3f));
	model[3]->model()->translate(GsVec(0.0f, -0.5f, 2.0f));


	harrier[0]->model()->translate(GsVec(12.0f, 6.35f, 0.0f));
	harrier[1]->model()->translate(GsVec(24.0f, 6.35f, 0.0f));
	harrier[2]->model()->translate(GsVec(36.0f, 6.35f, 0.0f));
	harrier[3]->model()->translate(GsVec(-12.0f, 6.35f, 0.0f));
	harrier[4]->model()->translate(GsVec(-24.0f, 6.35f, 0.0f));
	harrier[5]->model()->translate(GsVec(-36.0f, 6.35f, 0.0f));
	harrier[6]->model()->translate(GsVec(48.0f, 6.35f, 0.0f));

	//model[4]->model()->rotate(GsQuat::angle(180));
	//SunGroup
	//sun = new SnGroup;
	//s = new SnModel();
	//s->model()->load_obj("../model/sun.obj");
	//sun->separator(true);
	//sun->add(s);
	//s->model()->scale(0.01f);
	//s->model()->translate(GsVec(20, 40, -20));
	//compute_mappings(" ");
	//rootg()->add(sun);


	//HeliBody 
	heli->separator(true);
	heli->add(body = new SnTransform);
	heli->add(model[0]);
	model[0]->model()->flat(true);
	model[0]->color(GsColor(40, 252, 3));
	

	//Small Prop
	small_p->separator(true);
	small_p->add(saxis = new SnTransform);
	small_p->add(sa);
	small_p->add(sprop = new SnTransform);
	small_p->add(model[1]);
	model[1]->model()->flat(true);
	model[1]->color(GsColor::cyan);

	//Big Prop
	big_p->separator(true);
	big_p->add(baxis = new SnTransform);
	big_p->add(ba);
	big_p->add(bprop = new SnTransform);
	big_p->add(model[2]);
	model[2]->model()->flat(true);
	model[2]->color(GsColor(74, 3, 252));

	//HeliCarrier
	carrier->separator(true);
	carrier->add(c = new SnTransform);
	carrier->add(model[3]);
	
	carrier->add(p = new SnTransform);
	carrier->add(pad[0]);
	carrier->add(pad[1]);
	carrier->add(pad[2]);
	carrier->add(pad[3]);
	carrier->add(pad[4]);
	carrier->add(pad[5]);
	carrier->add(pad[6]);
	carrier->add(pad[7]);
	carrier->add(h = new SnTransform);
	carrier->add(harrier[0]);
	carrier->add(harrier[1]);
	carrier->add(harrier[2]);
	carrier->add(harrier[3]);
	carrier->add(harrier[4]);
	carrier->add(harrier[5]);
	carrier->add(harrier[6]);

	model[3]->model()->flat(true);
	model[3]->color(GsColor(98, 122, 89));

	//Given by the Professor
	//This part sets the material to be placed into the shadow models
	SnMaterial* shMaterial = new SnMaterial;
	GsMaterial m;
	m.diffuse = GsColor::black;
	shMaterial->material(m, 6);
	shMaterial->restore(true);

	shadowHeli->separator(true);
	shadowHeli->add(sH = new SnTransform);
	shadowHeli->add(shMaterial);
	shadowHeli->add(big_p);
	shadowHeli->add(heli);
	shadowHeli->add(small_p);
	
	
	////Adding the heli group into root.
	
	//t transform moves the entire helicopter
	//entire moves every object
	rootg()->add(entire = new SnTransform);
	rootg()->add_group(carrier);
	
	rootg()->add(t = new SnTransform);
	rootg()->add_group(heli);
	rootg()->add_group(small_p);
	rootg()->add_group(big_p);
	rootg()->add_group(shadowHeli);
	compute_mappings("");


}

void MyViewer::align_models() {

	GsMat sc, rot, trans;
	
	body->get().scaling(0.07f);
	bprop->get().scaling(1.5f);

	scaleBody(t, 0.1f);
	sc.scaling(0.5f);
	trans.translation(GsVec(0, 10.5f, 21.0f));
	saxis->get().mult(sprop->get(), sc * trans);

	trans.translation(GsVec(0.0f, 4.2f, -3.5f));
	rot.rotz(gspi / 2);
	
	baxis->get().mult(bprop->get(),trans*rot);

	sc.scaling(0.5f);
	rot.rotz(-0.005f);
	trans.translation(GsVec(0.0f, -5.7f, 0));
	c->get().mult(c->get(), rot*sc * trans);
	t->get().scaling(0.15f);
	trans.translation(GsVec(0, -offset, 0));
	computeShadow();
	entire->get().roty(gspi);

	sc.scaling(0.84f);
	rot.roty(gspi);
	h->get().mult(h->get(), sc * rot);
}

void MyViewer::compute_mappings(char* file) {

	for (int i = 0; i < 7; i++) {
		////Code Provided by the Professor in the PA5 Pdf.
		GsModel& m = *harrier[i]->model();
		//To reuse G to recompute texture.

		m.G.init();
		GsModel::Group& g = *m.G.push();
		g.fi = 0;
		g.fn = m.F.size();
		g.dmap = new GsModel::Texture;
		g.dmap->fname.set("../textures/Dirty.jpg");
		m.M.push().init();
		m.M.top() = GsMaterial::DefaultAmbient;

		int nv = m.V.size();
		m.T.size(nv);
		int k = 0;
		//Calculates the correct mapping to the image.
		//for (float i = 0; i < (float)nv; i+=1.0f) {
		//	float x = (float)i / (float)nv;
		//	float y = (float)i / (float)nv;
		//	//gsout << x << gsnl;
		//	m.T[(int)i].set(GsVec2(x, y));

		//}

		m.set_mode(GsModel::Hybrid, GsModel::PerGroupMtl);
		m.textured = true;
		//render();
	}
}

void MyViewer::build_pad() {
	for (int i = 0; i < 8; i++) {
		GsModel& m = *pad[i]->model();
		float x = 4.0f;
		//Vertices
		m.V.push() = GsVec(-x, 0, -x);
		m.V.push() = GsVec(-x, 0, x);
		m.V.push() = GsVec(x, 0, -x);
		m.V.push() = GsVec(-x, 0, x);
		m.V.push() = GsVec(x, 0, x);
		m.V.push() = GsVec(x, 0, -x);
		//Faces
		m.F.push() = GsModel::Face(0, 1, 2);
		m.F.push() = GsModel::Face(2, 3, 4);

		//Normals
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));

		//Textures, code given by Professor
		m.T.push() = GsVec2(0.0f, 1.0f);
		m.T.push() = GsVec2(0.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 1.0f);
		m.T.push() = GsVec2(0.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 1.0f);

		GsModel::Group& g = *m.G.push();

		g.fi = 0;
		g.fn = m.F.size();
		g.dmap = new GsModel::Texture;
		g.dmap->fname.set("../textures/helipad.jpg");
		m.M.push().init();
		m.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
		m.textured = true;
	}
}

void MyViewer::run_propellors(bool on) {

	//Code provided by Professor, using this for a consistent computation on any computers
	double t = 0, lt = 0, t0 = gs_time();
	double frdt = 1.0 / 60.0; //frames
	do {
		while (t - lt < frdt) { ws_check(); t = gs_time() - t0; }

		lt = gs_time() - t0;
		if (engine_on == false)break;
		//computeShadow();
		rotateX(baxis, -angle);
		rotateX(saxis, -angle);
		render();
		ws_check();
	} while (on == true);
}
void MyViewer::translateY(SnTransform* t,float y){
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(0, y, 0));
	mat.mult(mat, tr);
	
}
void MyViewer::translateX(SnTransform* t, float x) {
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(x, 0, 0));
	mat.mult(mat, tr);
	
}
void MyViewer::translateZ(SnTransform* t, float z) {
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(0, 0, z));
	mat.mult(mat, tr);
}
void MyViewer::translate(SnTransform* t,float x, float y, float z) {
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(x, y, z));
	mat.mult(mat, tr);
}
void MyViewer::rotateX(SnTransform* t, float r){
	GsMat& mat = t->get(), rot;
	rot.rotx(r);
	mat.mult(mat, rot);
}
void MyViewer::rotateY(SnTransform* t, float r) {
	GsMat& mat = t->get(), rot;
	rot.roty(r);
	mat.mult(mat, rot);
}
void MyViewer::rotateZ(SnTransform* t, float r) {
	GsMat& mat = t->get(), rot;
	rot.rotz(r);
	mat.mult(mat, rot);
}
void MyViewer::scaleBody(SnTransform* t, float s){
	GsMat& mat = t->get(), sc;
	sc.scaling(s);
	mat.mult(mat, sc);

}

void MyViewer::cameraMode(int mode) {
	GsMat tr, rot,rot1, rot2;
	//tr.translation(GsVec(0, 20, 0));
	rot.rotx(gspi / 227);
	rot1.roty(gspi / 365);
	rot2.rotz(gspi / 2);
	//rotationx.rotz(gspi/40);

	//rot.mult(tr, rot);
	switch (mode) {
		
	case 0: {
		//camera().init();
		camera().eye.y = 40;
		camera().eye.x = 0;
		camera().eye.z = 0;
		//camera().up.y = 100;
		//camera().fovy = GS_TORAD(60);
		//camera().aspect = 2;
		//camera().zfar = 1000;
		//camera().translate();
		render();
		ws_check();
		break;
	}
	case 1: {
		if (camera().eye.y == 40) {
			//Code for time part provided by Professor
			double t = 0, lt = 0, t0 = gs_time(); 
			double frdt = 1.0 / 60.0; //frames

			do
			{
				while (t - lt < frdt) { ws_check(); t = gs_time() - t0; }

				lt = gs_time() - t0;

				if (engine_on) {
					rotateX(baxis, -angle);
					rotateX(saxis, -angle);
				}
				if (lt < 3.0f) {
					/*camera().eye.x += 0.001f;
					camera().center.x   += 0.001f;
					camera().up.x += 0.001f;*/
					camera().rotate(rot);
				}
				if (lt > 3.0f) {
					/*camera().eye.y -= 0.001f;
					camera().center.y += 0.001f;
					camera().up.y += 0.001f;*/
					camera().rotate(rot1);

				}
				


				message().setf("local time = % f", lt);
				render();
				ws_check();

			
			} while (lt < 8.0f);
		}
		else cameraMode(cMode = 0);
		break;
	}
	}
	
}

void MyViewer::computeShadow() {

	GsMat s = GsMat(1.0f, (-q.x / q.y), 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, (-q.z / q.y), 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 1.0f);

	GsMat tr;
	tr.translation(GsVec(0, -offset, 0));
	sH->get().mult(tr, s);
	
	
	
}

void MyViewer::run_animation ()
{
	output("ANIMATING!");
	cameraMode(mode);
	_animating = false;
	options(-1);
	mode = mode ? false : true;
}

int MyViewer::handle_keyboard(const GsEvent& e)
{
	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;
	GsMat rot;
	int menu = 0;

	//will determine how fast each interaction is. Lower is bigger.
	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); break;

	case GsEvent::KeyEnter:{
		if (object == 0) {
			if (engine_on && altitude == 0) { options(3); engine_on = false; }
			else engine_on = true;
			options(2);
			run_propellors(engine_on);
		}
		break;
	}
	case GsEvent::KeyUp: 
	{
		if (object == 0) {
			if (engine_on) {
				if (!mode) {
					if (altitude >= 0) {
						translateY(t, speed);
						offset += speed;
						altitude += 1;
						options(6);
					}
				}
				else {
					options(6);
					translateZ(t, -speed);
				}
			}
			else options(1);
		}
		if (object == 1 && altitude > 0) {
			options(6);
			translateX(c, speed);
		}
		else if (object == 1 && altitude == 0) {
			options(6);
			translateX(entire, speed);
		}
		break;
	}

	case GsEvent::KeyDown: {
		if (object == 0) {
			if (engine_on) {
				if (!mode) {
					if (altitude >= 1) {
						translateY(t, -speed);
						offset -= speed;
						altitude -= 1;
						options(6);
					}
					else options(4);
				}
				else {
					options(6);
					translateZ(t, speed);
				}

			}
			else options(1);
		}
		if (object == 1 && altitude > 0) {
			options(6);
			translateX(c, -speed);
		}
		else if (object == 1 && altitude == 0) {
			options(6);
			translateX(entire, -speed);

		}
		break;
	}

	case GsEvent::KeyLeft: case 'd': {
		if (object == 0) {
			if (engine_on) {
				options(7);
				rotateY(t, gspi / 60);
			}
			else options(1);

		}
		if (object == 1 && altitude > 0) {
			options(7);
			rotateY(c, gspi / 60);
		}
		else if (object == 1 && altitude == 0) {
			options(7);
			rotateY(entire, gspi / 60);
		}
		break;
	}

	case GsEvent::KeyRight: case 'e': {
		if (object == 0) {
			if (engine_on) {
				options(7);
				rotateY(t, -gspi / 60);
			}
			else options(1);

		}
		if (object == 1 && altitude > 0) {
			options(7);
			rotateY(c, -gspi / 60);
		}
		else if (object == 1 && altitude == 0) {
			options(7);
			rotateY(entire, -gspi / 60);
		}
		break;
	}

	case 'q': {
		options(7);
		rotateX(baxis, gspi / 60);
		break;
	}

	case 'a': {			
		options(7);
		rotateX(baxis, -gspi / 60);
		break;
	}

	case 'w': {
		options(7);
		rotateX(saxis, gspi / 60);
		break;
	}

	case 's': {
		rotateX(saxis, -gspi / 60);
		break;
	}
	case 'z': {
		scaleBody(entire, 1.1f);
		options(8);
		break;
	}

	case 'x': {
		scaleBody(entire, 0.9f);
		options(8);
		break;
	}

	case 'm':
		options(9);
		mode = !mode;
		break;

	case ' ': {
		cameraMode(cMode);
		if (cMode == 0) cMode = 1;
		else cMode = 0;
		options(5);
		break;
	}
	case 'c': {
		if (object < 1) {
			object++;
		}
		else object = 0; //reset
		options(10);
		break;
	}

	default:
	{
		options(0);
	}

	}
	options(-1);
	computeShadow();
	render();
	ws_check();
	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{
	case EvAnimate: _animating = true; 
		//run_propellors(engine_on);
		run_animation(); options(0); return 1;

	case EvExit: gs_exit();

	default: {
		options(0);
	}
	
	}
	return WsViewer::uievent(e);
}
void MyViewer::spin_animation_occured() {
	message().setf("%f", fps());
}
int MyViewer::handle(const GsEvent& e)
{
	return WsViewer::handle(e);
}