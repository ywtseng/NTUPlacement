
#include "detailed.hpp"

#include "../util/const.hpp"
#include "sparse_matrix.hpp"
#include "vector.hpp"

#include <Eigen/SparseLU>
#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <stack>
#include <iomanip>

using namespace std;

Detailed::Detailed(Database& database): database_(database){
	subinstancematrix_.clear();
}

void Detailed::PreDetailedPlacement(){
	cout<<"============================================================"<<endl;
	cout<<"Initial DDA Constraint..."<<endl;
	InitialPlacementConstraint();
	cout<<"Find forbidden pair..."<<endl;
	FindForbiddenPair();
	cout<<"============================================================"<<endl;
	cout<<"Initial DetailedPlacement..."<<endl;
	
}
void Detailed::PreSATMethod(){
	SatProblem();
	//SatProblemPartition()
	SatInput();
	//SatInputPartition()
	cout<<"Output SAT input format..."<<endl;
}
void Detailed::PostSATMethod(){
	cout<<"Input SAT result format..."<<endl;
	SatOutput();
	//SatOutputPartition();
	FindForbiddenPair();
}

void Detailed::DPMethod(){
	DynamicProgramming();
	FindForbiddenPair();
}

void Detailed::InitialPlacementConstraint(){
	const double row_height = Site::height();
	const double site_width= Site::width();
	const double num_site_every_site = database_.num_sites()/database_.num_rows();
	cout<<"Chip : "<<row_height<<" row X "<<num_site_every_site<<" site"<<endl;
	//Sort initial instance order in every row_height
	//Set left instance of every instance 
	for(int i = 0; i < database_.num_rows(); ++i){
		vector<SubInstanceId> everyrow_subinstanceid;
		vector<SiteId> everyrow_siteid;

		SubInstanceId left_subinstance_id = (SubInstanceId)-1;
		for(int j = i * num_site_every_site; j < ((i+1)* num_site_every_site) ;++j) {
			const SiteId site_id(j);
			Site& site = database_.site(site_id);
			site.set_site_id(site_id);
			everyrow_siteid.push_back(site_id);
			if(site.has_sub_instance()){
				const SubInstanceId subinstance_id = site.sub_instance_id();
				SubInstance& subinstance = database_.sub_instance(subinstance_id);
				if(left_subinstance_id!=subinstance_id){
					subinstance.set_left_subinstance(left_subinstance_id);
					everyrow_subinstanceid.push_back(site.sub_instance_id());
					left_subinstance_id = subinstance_id;
				}
			}
		}
		SubInstanceId right_subinstance_id = (SubInstanceId)-1;
		for(int j = (everyrow_subinstanceid.size()-1); j >= 0 ;j--){
			const SubInstanceId subinstance_id = everyrow_subinstanceid[j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			subinstance.set_right_subinstance(right_subinstance_id);
			right_subinstance_id = subinstance_id;
		}
		subinstancematrix_.push_back(everyrow_subinstanceid);
		sitematrix_.push_back(everyrow_siteid);
	}
	
	//Set DDA constraint
	for(int i = 0; i <subinstancematrix_.size();i++){
		for(int j = 0; j < subinstancematrix_[i].size(); j++){
			const SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			bool LT,LB,RT,RB;
			switch(rand() % 8){
				case 0:
					LT = true; LB = false; RT = true; RB = true; 
					break;
				case 1:
					LT = true; LB = true; RT = true; RB = false;
					break;
				case 2:
					LT = true; LB = true; RT = false; RB = true;
					break;
				case 3:
					LT = false; LB = false ;RT = true; RB = true;
					break;
				case 4:
					LT = true; LB = true; RT = true; RB = false;
					break;
				default:
					LT = true; LB = true; RT = true; RB = true;
					break;
			}
			subinstance.set_gates_left(LT,LB);
			subinstance.set_gates_right(RT,RB);
			
		}
	}
	//Set OD constraint
	for(int i = 0; i <subinstancematrix_.size();i++){
		for(int j = 0; j < subinstancematrix_[i].size(); j++){
			const SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			switch(rand() % 2){
				case 0:
					subinstance.set_oxideheight(1.0);
					break;
				case 1:
					subinstance.set_oxideheight(2.0);
					break;
				default:
					subinstance.set_oxideheight(1.0);
					break;
			}
		}
	}	
}

void Detailed::DynamicProgramming(){
	double total_displacement = 0;
	for(int i = 0; i < subinstancematrix_.size(); i++){	
		vector<SubInstanceId>& subinstance_row = subinstancematrix_[i];
		//cout<<i<<" = "<<subinstance_row.size()<<endl;
		for(int j = 1; j < (subinstance_row.size()-2) ; j++ ){
			SubInstanceId A_id = subinstance_row[j-1];
			SubInstanceId B_id = subinstance_row[j];
			SubInstanceId C_id = subinstance_row[j+1];
			SubInstanceId D_id = subinstance_row[j+2];
			SubInstance& A = database_.sub_instance(A_id);
			SubInstance& B = database_.sub_instance(B_id);
			SubInstance& C = database_.sub_instance(C_id);
			SubInstance& D = database_.sub_instance(D_id);
			if(B.dda_forbidden){
				switch(test_dda_forbidden(A,B,C,D)){
					case 1 :
						B.flipped();
						//flipped_cell(B);
						break;
					case 2 :
						C.flipped();
						//flipped_cell(C);
						break;
					case 3 :
						B.flipped();
						C.flipped();
						//flipped_cell(B);
						//flipped_cell(C);
						break;
					case 4 :
						swap_cell(B,C,total_displacement);
						subinstance_row[j] = C_id;
						subinstance_row[j+1] = B_id ;
						break;
					case 5 :
						swap_cell(B,C,total_displacement);
						subinstance_row[j] = C_id;
						subinstance_row[j+1] = B_id ;
						C.flipped();
						//flipped_cell(C);
						break;
					case 6 :
						swap_cell(B,C,total_displacement);
						subinstance_row[j] = C_id;
						subinstance_row[j+1] = B_id ;
						B.flipped();
						//flipped_cell(B);
						break;
					case 7 :
						swap_cell(B,C,total_displacement);
						subinstance_row[j] = C_id;
						subinstance_row[j+1] = B_id ;
						C.flipped();
						//flipped_cell(C);
						break;
					default :
						break;
				}
			}
			/*else if(B.od_forbidden){
				switch(test_od_forbidden(A,B,C,D)){
					case 1 :
						swap_cell(B,C);
						subinstance_row[j] = C_id;
						subinstance_row[j+1] = B_id ;
						break;
					default :
						break;
				}
			}*/
		}
	}
	cout<<"#  Total_displacement:  "<<total_displacement<<endl;
	for(int i = 0; i < database_.num_sub_instances(); i++){
		SubInstanceId sub_instance_id(i);
		SubInstance& sub_instance = database_.sub_instance(sub_instance_id);
		InstanceId instance_id = sub_instance.instance_id();
		Instance& instance = database_.instance(instance_id);
		instance.set_position(Point(sub_instance.position().x(),instance.position().y()));
		
	}
}


void Detailed::SatProblem(){
	//build first variable
	database_.add_variable(Variable());
	
	for(int i = 0; i < subinstancematrix_.size(); i++){
		int total_variable = 0 ;
		for(int j = 0 ; j < subinstancematrix_[i].size();j++){
			const SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			InstanceId instance_id = subinstance.instance_id();
			Instance& instance = database_.instance(instance_id);
			VariableId variable_id;
			VariableId flip_variable_id; 
			//cout<<i<<"  "<<j<<endl;
			if(instance.num_variables() == 0){
				variable_id = database_.add_variable(Variable(instance_id,false,instance.position()));
				flip_variable_id = database_.add_variable(Variable(instance_id,true,instance.position()));	
				instance.add_variable_id(variable_id);
				instance.add_variable_id(flip_variable_id);
				total_variable++;
				total_variable++;
			} else {
				variable_id = instance.variable_id(0);
				flip_variable_id = instance.variable_id(1);
			}
			
			
			//cout<<(int)variable_id<<"   "<<(int)flip_variable_id<<endl;
			
			SiteId site_id = database_.site_id_by_position(subinstance.position());
			Site& site = database_.site(site_id);
		
			site.add_variable_id(variable_id);
			site.add_variable_id(flip_variable_id);
		}
		numvariable_.push_back(total_variable);
	}
}

void Detailed::SatInput(){
	vector< vector<int> >   forbidden_clause;
	int total_clause = 0 ;
	int contraint_one = 0 ;
	int contraint_two = 0 ;
	int contraint_three = 0 ;
	for(int i = 0; i < subinstancematrix_.size(); i++){
		//constraint 1
		for(int j = 0 ; j < subinstancematrix_[i].size();j++){
			SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			InstanceId instance_id = subinstance.instance_id();
			Instance& instance = database_.instance(instance_id);
			
			if(instance.is_set_variable == false){
				vector<int> clause;
				vector<int> inv_clause;
				for(int k = 0; k < instance.num_variables() ;k++){
					VariableId variable_id = instance.variable_id(k);
					//cout<<(int)variable_id<<"  ";
					clause.push_back((int)variable_id);
					inv_clause.push_back(-(int)variable_id);
				}
				//cout<<endl;
				contraint_one++;
				contraint_one++;
				forbidden_clause.push_back(clause);
				forbidden_clause.push_back(inv_clause);
				instance.is_set_variable = true;
			}
		}
		//constraint 2
		for(int j = 0 ; j < sitematrix_[i].size(); j++){
			SiteId site_id = sitematrix_[i][j];
			Site& site = database_.site(site_id);
			if(site.num_variables()!=0){
				vector<int> clause;
				for(int k = 0; k < site.num_variables(); k++){
					VariableId variable_id = site.variable_id(k);
					clause.push_back(-(int)variable_id);
				}
				contraint_two++;
				forbidden_clause.push_back(clause);
			}
		}
		//constraint 3
		for(int j = 0; j < (subinstancematrix_[i].size()-1); j++){
			SubInstanceId subinstance_id_A = subinstancematrix_[i][j];
			SubInstanceId subinstance_id_B = subinstancematrix_[i][j+1];

			SubInstance& A = database_.sub_instance(subinstance_id_A);
			SubInstance& B = database_.sub_instance(subinstance_id_B);
			
			InstanceId instance_id_A = A.instance_id();
			Instance& A_i = database_.instance(instance_id_A);
			InstanceId instance_id_B = B.instance_id();
			Instance& B_i = database_.instance(instance_id_B);
			
			for(int k = 0; k < A_i.num_variables();k++){
				VariableId A_id = A_i.variable_id(k);
				Variable& A_a = database_.variable(A_id);
				bool test1;
				bool test2;
				if(A_a.flipped()){
					test1 = A.lefttop();
					test2 = A.leftbottom();
				} else {
					test1 = A.righttop();
					test2 = A.rightbottom();					
				}
				
				for(int m = 0; m < B_i.num_variables();m++){
					VariableId B_id = B_i.variable_id(m);
					Variable& B_b = database_.variable(B_id);
					bool test3;
					bool test4;
					if(B_b.flipped()){
						test3 = B.righttop();
						test4 = B.rightbottom();
					} else {
						test3 =  B.lefttop();
						test4 =  B.leftbottom();					
					}
					if(judge_dda_pair(test1,test2,test3,test4)){
						vector<int> clause;
						clause.push_back(-(int)A_id);
						clause.push_back(-(int)B_id);
						contraint_three++;
						forbidden_clause.push_back(clause);
					}
				}
			}	
		}
	}
	cout<<"contraint_1 = "<<contraint_one<<endl;
	cout<<"contraint_2 = "<<contraint_two<<endl;
	cout<<"contraint_3 = "<<contraint_three<<endl;
	int total_variable = 0;
	for(int i = 0;i < numvariable_.size();i++){
		total_variable = (total_variable+numvariable_[i]);
	}
	//total output
	fstream fp;
	string output = "./pre/input";
	string numtxt;
	stringstream ss;
	//ss << i; 
	//ss >>numtxt;
	
	//string outputfile = (output+numtxt);
	string outputfile = (output);
	fp.open(outputfile.c_str(),ios::out);
	
	fp<<"p cnf "<<total_variable<<" "<<forbidden_clause.size()<<endl;
	for(int j = 0 ; j < forbidden_clause.size();j++){
		vector<int> clause = forbidden_clause[j];
		for(int k = 0;k < clause.size(); k++){
			fp<<clause[k]<<" ";
		}
		fp<<"0"<<endl;
	}
}

void Detailed::SatOutput(){
	ifstream fp;
	string output = "./post/output";
	stringstream ss;
	string outputfile = (output);
	fp.open(outputfile.c_str(),ios::in);
	if(!fp){ 
        cout<<"Fail to open file: "<<outputfile<<endl;
		return;
    }
	string text;
	bool is_start = false; 
	while(is_start == false){
		getline(fp,text);
		if(text[0] == 's'){
			is_start = true;
		}
	}
	//output result
	vector<int> satresult;
	while(fp>>text){
		satresult.push_back(atoi(text.c_str()));
	}
	for(int i = 0;i < satresult.size();i++){
		if(satresult[i] > 0 ){
			VariableId variableid = (VariableId)satresult[i];
			Variable& variable = database_.variable(variableid);
			variable.selected = true;
		}
	}		
	for(int i = 0 ; i < subinstancematrix_.size();i++){
		for(int j = 0 ; j < subinstancematrix_[i].size();j++){
			SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			InstanceId instance_id = subinstance.instance_id();
			Instance& instance = database_.instance(instance_id);
			for(int k = 0 ; k < instance.num_variables();k++){
				VariableId variable_id = instance.variable_id(k);
				Variable& variable = database_.variable(variable_id);
				if(variable.selected == true){
					bool is_flipped = variable.flipped();
					if(is_flipped){
						subinstance.flipped();
					}
				}
			}
		}
	}
}


void Detailed::SatProblemPartition(){
	//build first variable
	database_.add_variable(Variable());
	for(int i = 0; i < subinstancematrix_.size(); i++){
	//for(int i = 0; i < 1; i++){
		int total_variable = 0 ;
		for(int j = 0 ; j < subinstancematrix_[i].size();j++){
			const SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			VariableId variable_id = database_.add_variable(Variable(subinstance_id,false,subinstance.position()));
			VariableId flip_variable_id = database_.add_variable(Variable(subinstance_id,true,subinstance.position()));
			//cout<<(int)variable_id<<"   "<<(int)flip_variable_id<<endl;
			subinstance.add_variable_id(variable_id);
			subinstance.add_variable_id(flip_variable_id);
			total_variable++;
			total_variable++;
			
			SiteId site_id = database_.site_id_by_position(subinstance.position());
			Site& site = database_.site(site_id);
		
			site.add_variable_id(variable_id);
			site.add_variable_id(flip_variable_id);
		}
		numvariable_.push_back(total_variable);
	}	
}

void Detailed::SatInputPartition(){
	vector< vector<int> >   forbidden_clause;
	
	for(int i = 0; i < subinstancematrix_.size(); i++){
		//constraint 1
		for(int j = 0 ; j < subinstancematrix_[i].size();j++){
			SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			vector<int> clause;
			vector<int> inv_clause;
			for(int k = 0; k < subinstance.num_variables() ;k++){
				VariableId variable_id = subinstance.variable_id(k);
				//cout<<(int)variable_id<<"  ";
				clause.push_back((int)variable_id);
				inv_clause.push_back(-(int)variable_id);
			}
			//cout<<endl;
			forbidden_clause.push_back(clause);
			forbidden_clause.push_back(inv_clause);
			//instance.is_set_variable = true;

		}
		//constraint 2
		for(int j = 0 ; j < sitematrix_[i].size(); j++){
			SiteId site_id = sitematrix_[i][j];
			Site& site = database_.site(site_id);
			if(site.num_variables()!=0){
				vector<int> clause;
				for(int k = 0; k < site.num_variables(); k++){
					VariableId variable_id = site.variable_id(k);
					clause.push_back(-(int)variable_id);
				}
				forbidden_clause.push_back(clause);
			}
		}
		//constraint 3
		
		for(int j = 0; j < (subinstancematrix_[i].size()-1); j++){
			
			SubInstanceId subinstance_id_A = subinstancematrix_[i][j];
			SubInstanceId subinstance_id_B = subinstancematrix_[i][j+1];

			SubInstance& A = database_.sub_instance(subinstance_id_A);
			SubInstance& B = database_.sub_instance(subinstance_id_B);
				
			for(int k = 0; k < A.num_variables();k++){
				VariableId A_id = A.variable_id(k);
				Variable& A_a = database_.variable(A_id);
				bool test1;
				bool test2;
				if(A_a.flipped()){
					test1 = A.lefttop();
					test2 = A.leftbottom();
				} else {
					test1 = A.righttop();
					test2 = A.rightbottom();					
				}
				
				for(int m = 0; m < B.num_variables();m++){
					VariableId B_id = B.variable_id(m);
					Variable& B_b = database_.variable(B_id);
					bool test3;
					bool test4;
					if(B_b.flipped()){
						test3 = B.righttop();
						test4 = B.rightbottom();
					} else {
						test3 =  B.lefttop();
						test4 =  B.leftbottom();					
					}
					if(judge_dda_pair(test1,test2,test3,test4)){
						vector<int> clause;
						clause.push_back(-(int)A_id);
						clause.push_back(-(int)B_id);
						forbidden_clause.push_back(clause);
					}
				}
			}	
		}
		
	}
	
	int total_variable = 0;
	for(int i = 0;i < numvariable_.size();i++){
		total_variable = (total_variable+numvariable_[i]);
	}
	//total output
	
	fstream fp;
	string output = "./pre/input";
	//string numtxt;
	stringstream ss;
	//ss << i; 
	//ss >>numtxt;
	
	//string outputfile = (output+numtxt);
	string outputfile = (output);
	fp.open(outputfile.c_str(),ios::out);
	
	fp<<"p cnf "<<total_variable<<" "<<forbidden_clause.size()<<endl;
	for(int j = 0 ; j < forbidden_clause.size();j++){
		vector<int> clause = forbidden_clause[j];
		for(int k = 0;k < clause.size(); k++){
			fp<<clause[k]<<" ";
		}
		fp<<"0"<<endl;
	}
}

void Detailed::SatOutputPartition(){
	for(int i = 0 ; i < subinstancematrix_.size();i++){
		ifstream fp;
		string output = "./post/output";
		string numtxt;
		stringstream ss;
		ss << i; 
		ss >>numtxt;
		string outputfile = (output +numtxt);
		fp.open(outputfile.c_str(),ios::in);
		string text;
		bool is_start = false; 
		while(is_start == false){
			getline(fp,text);
			if(text[0] == 's'){
				is_start = true;
			}
		}
		//output result
		vector<int> satresult;
		while(fp>>text){
			satresult.push_back(atoi(text.c_str()));
		}
		for(int i = 0;i < satresult.size();i++){
			if(satresult[i] > 0 ){
				VariableId variableid = (VariableId)satresult[i];
				Variable& variable = database_.variable(variableid);
				variable.selected = true;
			}
		}		
	}
	for(int i = 0 ; i < subinstancematrix_.size();i++){
		for(int j = 0 ; j < subinstancematrix_[i].size();j++){
			SubInstanceId subinstance_id = subinstancematrix_[i][j];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			for(int k = 0 ; k < subinstance.num_variables();k++){
				VariableId variable_id = subinstance.variable_id(k);
				Variable& variable = database_.variable(variable_id);
				if(variable.selected == true){
					bool is_flipped = variable.flipped();
					if(is_flipped){
						subinstance.flipped();
					}
				}
			}
		}
	}
}

int Detailed::test_dda_forbidden(SubInstance A,SubInstance B,SubInstance C,SubInstance D){
	bool N1  = A.righttop();
	bool N2  = A.rightbottom();
	bool N3  = B.lefttop();
	bool N4  = B.leftbottom();
	bool N5  = B.righttop();
	bool N6  = B.rightbottom();
	bool N7  = C.lefttop();
	bool N8  = C.leftbottom();
	bool N9  = C.righttop();
	bool N10 = C.rightbottom();
	bool N11 = D.lefttop();
	bool N12 = D.leftbottom();
	
	bool test1 = false;
	bool test2 = false;
	bool test3 = false;
	bool test4 = false;
	bool test5 = false;
	bool test6 = false;
	bool test7 = false;
	if(judge_dda_pair(N1,N2,N5,N6)==false &&
		judge_dda_pair(N3,N4,N7,N8)==false &&
		judge_dda_pair(N9,N10,N11,N12)==false){
			test1 = true;
		}
	if(judge_dda_pair(N1,N2,N3,N4)==false &&
		judge_dda_pair(N5,N6,N9,N10)==false &&
		judge_dda_pair(N7,N8,N11,N12)==false){
			test2 = true;
		}
	if(judge_dda_pair(N1,N2,N5,N6)==false &&
		judge_dda_pair(N3,N4,N9,N10)==false &&
		judge_dda_pair(N7,N8,N11,N12)==false){
			test3 = true;
		}
	if(judge_dda_pair(N1,N2,N7,N8)==false &&
		judge_dda_pair(N9,N10,N3,N4)==false &&
		judge_dda_pair(N5,N6,N11,N12)==false){
			test4 = true;
		}
	if(judge_dda_pair(N1,N2,N9,N10)==false &&
		judge_dda_pair(N7,N8,N3,N4)==false &&
		judge_dda_pair(N5,N6,N11,N12)==false){
			test5 = true;
		}
	if(judge_dda_pair(N1,N2,N7,N8)==false &&
		judge_dda_pair(N9,N10,N5,N6)==false &&
		judge_dda_pair(N3,N4,N11,N12)==false){
			test6 = true;
		}
	if(judge_dda_pair(N1,N2,N9,N10)==false &&
		judge_dda_pair(N7,N8,N5,N6)==false &&
		judge_dda_pair(N3,N4,N11,N12)==false){
			test7 = true;
		}
		
	if(test1){ return 1; }
	else if(test2){ return 2; }
	else if(test3){ return 3; }
	else if(test4){ return 4; }
	else if(test5){ return 5; }
	else if(test6){ return 6; }
	else if(test7){ return 7; }
	else { return 8; }
}

int Detailed::test_od_forbidden(SubInstance A,SubInstance B,SubInstance C,SubInstance D){
	if(judge_od_pair(C,B)){
		return 1;
	} else {
		return 0;
	}
}

void Detailed::flipped_cell(SubInstance& sub_cell){
	InstanceId instance_id = sub_cell.instance_id();
	Instance& instance = database_.instance(instance_id);
	for(int i = 0 ;i<instance.num_sub_instances();i++){
		SubInstanceId sub_instance_id(i);
		SubInstance& sub_instance = database_.sub_instance(sub_instance_id);
		sub_instance.flipped();
	}
	
}

void Detailed::swap_cell(SubInstance& B,SubInstance& C,double& total_displacement){
	double temp_B = B.position().x();
	double temp_C = C.position().x();
	B.set_position(Point(temp_C, B.position().y()));
	C.set_position(Point(temp_B, C.position().y()));
	total_displacement = (total_displacement+2*fabs(temp_B-temp_C));
	/*InstanceId instance_id_b = B.instance_id();
	InstanceId instance_id_c = C.instance_id();
	Instance& instance_B = database_.instance(instance_id_b);
	Instance& instance_C = database_.instance(instance_id_c);*/
}

void Detailed::FindForbiddenPair(){
	int total_pair = 0;
	int total_forbidden_pair = 0 ;
	int total_dda_pair = 0 ;
	int total_od_pair = 0 ;
	//Clear to 0
	for (int i = 0; i < database_.num_instances(); ++i) {
		const InstanceId instance_id(i);
		Instance& instance = database_.instance(instance_id);
		instance.forbidden_cell = false;
	}
	//for(int i = 0; i < 1;i++){
	for(int i = 0; i <subinstancematrix_.size();i++){
		for(int j = 0; j < (subinstancematrix_[i].size()-1); j++){
			const SubInstanceId subinstance_id = subinstancematrix_[i][j];
			const SubInstanceId next_subinstance_id = subinstancematrix_[i][j+1];
			SubInstance& subinstance = database_.sub_instance(subinstance_id);
			SubInstance& next_subinstance = database_.sub_instance(next_subinstance_id);
			bool dda = judge_dda_pair(subinstance.righttop(),subinstance.rightbottom()
				,next_subinstance.lefttop(),next_subinstance.leftbottom());
			//bool od = judge_od_pair(subinstance,next_subinstance);
			//if(dda || od){
			if(dda){
				const InstanceId instance_id = subinstance.instance_id();
				Instance& instance = database_.instance(instance_id);
				const InstanceId next_instance_id = next_subinstance.instance_id();
				Instance& next_instance = database_.instance(next_instance_id);
				instance.forbidden_cell = true;
				next_instance.forbidden_cell = true;
				total_forbidden_pair++;
				if(dda){ 
					subinstance.dda_forbidden = true;
					next_subinstance.dda_forbidden = true;
					total_dda_pair++; 
				}
				/*if(od){ 
					subinstance.od_forbidden = true;
					next_subinstance.od_forbidden = true;
					total_od_pair++; 
				}*/
			}
			total_pair++;
		}
	}
	cout<<"# of Check Pairs = "<<total_pair<<endl;
	//cout<<"# of Forbidden Pairs = "<<total_forbidden_pair<<endl;
	cout<<"# of DDA Pairs = "<<total_dda_pair<<endl;
	//cout<<"# of OD Pairs = "<<total_od_pair<<endl;
	//cout<<"  Ratio of Forbidden Subinstance: "<<(double)total_forbidden_pair/(double)total_pair<<endl;
	//Static Sum information
	int total_forbidden_instance = 0 ;
	int single_forbbiden_instance = 0 ;
	int double_forbbiden_instance = 0 ;
	int triple_forbbiden_instance = 0 ;
	int quad_forbbiden_instance = 0 ;
	for (int i = 0; i < database_.num_instances(); ++i) {
		const InstanceId instance_id(i);
		Instance& instance = database_.instance(instance_id);
		if(instance.forbidden_cell == true ){
			total_forbidden_instance++;
			if(instance.height() == Site::height()  ){ single_forbbiden_instance++; }
			if(instance.height() == Site::height()*2){ double_forbbiden_instance++; }
			if(instance.height() == Site::height()*3){ triple_forbbiden_instance++; }
			if(instance.height() == Site::height()*4){ quad_forbbiden_instance++; }
		}
	}
	cout<<"# of Instances: "<<database_.num_instances()<<endl;
	cout<<"# of Forbidden Instances: "<<total_forbidden_instance<<endl;
	cout<<"       # of 1-Row-Height: "<<single_forbbiden_instance<<endl;
	cout<<"       # of 2-Row-Height: "<<double_forbbiden_instance<<endl;
	cout<<"       # of 3-Row-Height: "<<triple_forbbiden_instance<<endl;
	cout<<"       # of 4-Row-Height: "<<quad_forbbiden_instance<<endl;
	cout<<"===> Ratio of Forbidden Instances: "<<(double)total_forbidden_instance/(double)database_.num_instances()<<endl;
}

bool Detailed::judge_dda_pair(bool rt,bool rb,bool lt,bool lb){
	int testing = 0;
	if(rt == true || lt ==true ){
		testing++;
	}
	if(rb ==true || lb == true){
		testing++;
	}
	
	if(testing == 2){
		return false;
	} else{
		return true;
	}
	
}

bool Detailed::judge_od_pair(SubInstance& subinstance,SubInstance& next_subinstance){
	double diff = fabs(next_subinstance.position().x()-subinstance.position().x());
	if(diff >= (Site::width()*4)){
		return false;
	}
	if(subinstance.oxideheight()==next_subinstance.oxideheight()){
		return false;
	} else {
		return true;
	}
}