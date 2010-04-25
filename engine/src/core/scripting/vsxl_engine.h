#ifndef VSXL_ENGINE_H_
#define VSXL_ENGINE_H_

class vsxl_engine {
public:
	gmMachine machine;
	gmVariable vsx_vtime;
	gmVariable vsx_dtime;
	int pf_id; // counter to make unique parameter filter functions
	int cf_id;
	vsxl_engine() : pf_id(0), cf_id(0)
	{}
	void init() {
    //machine = new gmMachine;
  	printf("float::load2\n");
    //GameObject::s_typeId = machine->CreateUserType("GameObject");
  	printf("float::load2\n");
    //machine->RegisterUserCallbacks(GameObject::s_typeId,GCTrace, GCDestruct,AsString);
  	printf("float::load2\n");
  	gmBindMathLib(&machine);
    printf("float::load3\n");		
	}
};

#endif /*VSXL_ENGINE_H_*/
