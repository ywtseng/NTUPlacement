#ifndef DETAILED_HPP
#define DETAILED_HPP

#include "../database/database.hpp"

class Detailed {
	public:
		Detailed(Database& database);
		//Pre Detailed Placement
		void PreDetailedPlacement();
		//SAT
		void PreSATMethod();
		void PostSATMethod();
		//Dynamic Programming
		void DPMethod();
	private:
		//Pre Detailed Placement
		void InitialPlacementConstraint();
		void FindForbiddenPair();
		//SAT
		void SatProblem();
		void SatInput();
		void SatOutput();
		//SATPartition
		void SatProblemPartition();
		void SatInputPartition();
		void SatOutputPartition();
		//Dynamic Programming
		void DynamicProgramming();
		
		//subfunction
		bool judge_dda_pair(bool rt,bool rb,bool lt,bool lb);
		bool judge_od_pair(SubInstance& subinstance,SubInstance& next_subinstance);
		int test_dda_forbidden(SubInstance A,SubInstance B,SubInstance C,SubInstance D);
		int test_od_forbidden(SubInstance A,SubInstance B,SubInstance C,SubInstance D);
		void swap_cell(SubInstance& B,SubInstance& C,double& total_displacement);
		void flipped_cell(SubInstance& sub_cell);
		
		std::vector< std::vector<SubInstanceId> > 						subinstancematrix_;
		std::vector< std::vector<SiteId> > 								sitematrix_;
		std::vector< int > 												numvariable_;
		
		Database& database_;
};





#endif