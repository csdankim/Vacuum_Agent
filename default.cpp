/********************************************************************************************************
 *													*
 *													*
 *      Randomized Reflex Agent Version 1.0								*
 *      File: random_no_wall.cpp									*
 *      												*
 *	Input: input_no_wall.txt									*
 *	Output: simple_no_wall.txt									*
 *	Output: randomized_no_wall.txt									*
 *	Output: model_no_wall.txt									*
 *													*
 *	As an input, 10*10 matrix is given, which includes walls, doors.				*
 *	0: Clean, 1: Dirty, 2: Wall, 3: Door								*
 * 													*
 *      grid[0][n], grid[n][0], grid[11][n], grid[n][11]:  Border of Room, n= 0 to 11			*
 *													*
 * 	"attempt" (in "randomized_reflex_agent()") : How many times we will repeat the same experiment	* 
 * 	the value is set to 50 times by default.							*
 *													*
 * 	"action_threshold" (global variable) : The total number of action taken, 			*
 * 	the value is set to 1000 by default. 								*
 * 	 												*
 * 													*
 *													*
 ********************************************************************************************************/

#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>
// initialized as -1, clean =0,  dirt =1, wall =2, door = 3
int readgrid[12][12] = {-1,};
int grid[12][12] = { -1,};
int resultgrid[500][12][12] = {-1, };
int wallmap[12][12] = {-1,};
int cleanmemory[12][12] = {-1,};

int wallcnt =0;
bool wallsensor = false;
bool dirtsensor = false;
bool homesensor = false;

int actioncount = 0; // for result
int goforward_go_cnt = 0; // the number of "go forward" and then 'go'
int goforward_notgo_cnt = 0; // the number of "go forward" but couldn't go due to wall (or the end)
int turnleft_cnt = 0; // the number of "turn left"
int turnright_cnt = 0; // the number of "turn right"
int suckup_cnt = 0; // the number of "suck up"

int posx_robot = 10;
int posy_robot = 1; // initial point (x:1,y:10) (bottom leftmost)
int dir_robot = 0; // north = 0,  east = 1, south = 2, west =3
int action = 0;
int xx, yy, wall = 0; //size

struct flag{
	int a:1; // dust state on current grid
	int b:1; 
	int c:1; 
	// bc:00 previous action is 'turn left'
	// bc:01 previous action is 'turn right'
};	// bc:10 previous action is 'go forward'


int action_threshold = 2000;

int randomaction();
int frontgrid_togocheck(int x, int y, int action, int dir_robot);
int clean_count_check();

void initialize_variables(); // initialize variables

bool wall_sensor_percept(int x, int y, int dir);
bool dirt_sensor_percept(int x, int y);
bool home_sensor_percept(int x, int y);

bool right_grid_check(int x, int y, int dir);
bool right_grid_state_check(int x, int y, int dir);
bool left_grid_check(int x, int y, int dir);
bool left_grid_state_check(int x, int y, int dir);


int randomized_reflex_agent();
int simple_deterministic_agent();
int model_based_deterministic_agent(); 
			
int main()
{


	freopen("input_no_wall.txt", "r", stdin);
	// read grid map
	/*int xx, yy;
	scanf("%d %d",&xx, &yy );
	for(int i =1 ; i<=xx; i++){
		for(int j =1; j<=yy; j++){
		scanf("%d ", &grid[i][j]);
		//printf("%d ", grid[i][j]);
		if(grid[i][j]==2)
			wallcnt++; // the number of the wall
		}
		//printf("\n");
	}*/


	scanf("%d %d", &xx, &yy);
	for (int i = 1; i <= xx; i++) {
		for (int j = 1; j <= yy; j++) {
			scanf("%d ", &readgrid[i][j]);
			grid[i][j] = readgrid[i][j];
			//printf("%d ", grid[i][j]);
			if (grid[i][j] == 2)
				wall++; // the number of the wall
		}
	}


//	int nresult = mkdir("result_default", S_IFDIR|S_IRWXU|S_IXOTH|S_IROTH);	
//	if(nresult != -1){
	FILE *simple_stream = freopen("simple_default.csv", "w", stdout);
	initialize_variables();
	simple_deterministic_agent();
	fclose(simple_stream);

	FILE *randomized_stream = freopen("random_default.csv", "w", stdout);
	initialize_variables();
	randomized_reflex_agent();
	fclose(randomized_stream);

	FILE *model_stream = freopen("model_default.csv", "w", stdout);
	initialize_variables();
	model_based_deterministic_agent();
	fclose(model_stream);
//	}
//
		
	/*for(int i=1; i<=10; i++){
		for(int j =1; j<=10; j++){
			printf("%d ",grid[i][j]);
		}
		printf("\n");
	}*/

	return 0;
}

int model_action_50percent()
{
	int act = (rand()%2 +1);
	return act;
}

int model_action_one_third()
{
	int act = (rand()%3 +1);
	return act;
}
int simple_randomaction()
{
	int act = (rand()%3 + 1);
	return act;
}
int simple_lr_randomaction()
{
	int act = (rand()%2 + 2);
	return act;
}
int randomaction()
{
	int act = (rand()%4 +1);
	return act;
}

// go forward =1
// turn right =2
// turn left =3
// suck up dirt =4
// turn off =5
// 
int frontgrid_togocheck(int x, int y, int action, int dir_robot)
{
	int gocheck = 1; //go=1, notgo=0
	

	if(action ==1 && dir_robot==0){	// to check if robot can go northbound
		if (posx_robot == 1) 
			gocheck =0;
	}
	if(action ==1 && dir_robot==1){	// to check if robot can go eastbound
		if (posy_robot == 10) 
			gocheck =0;
	}
	if(action ==1 && dir_robot==2){ // to check if robot can go southbound
		if (posx_robot == 10) 
			gocheck =0;
	}
	if(action ==1 && dir_robot==3){ // to check if robot can go westbound 
		if (posy_robot == 1) 
			gocheck =0;
	}

	return gocheck ;	
}

// return the number of clean grid
int clean_count_check()
{

	int cnt = 0;
	
	for (int i = 1; i <= 10; i++) {
		for (int j = 1; j <= 10; j++) {
			if (grid[i][j] == 0)
				cnt++;
		}
	}
	return cnt;
}

void initialize_variables()
{
	action_threshold = 2000;
	posx_robot = 10;
	posy_robot = 1;
	dir_robot = 0; // north = 0,  east = 1, south = 2, west =3
	wallcnt = 0;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			grid[i][j] = readgrid[i][j];
			wallmap[i][j] = -1;
			cleanmemory[i][j] = -1;
		}
	}
	
	for(int k =0; k<50; k++){
		for(int i=1; i<=10; i++){
			for(int j=1; j<=10; j++){
				resultgrid[k][i][j] = 0;
			}
		} 
	}
	wallsensor = false;
	dirtsensor = false;
	homesensor = false;

	actioncount = 0; // for result
	action = 0;
	goforward_go_cnt = 0; // the number of "go forward", => go
	goforward_notgo_cnt = 0; // the number of "go forward" => could not go
	turnleft_cnt = 0; // the number of "turn left"
	turnright_cnt = 0; // the number of "turn right"
	suckup_cnt = 0; // the number of "suck up"
}

bool wall_sensor_percept(int x, int y, int dir)
{
	wallsensor = false; 

	if(dir ==0){ // northbound
		if(x == 1){
			wallsensor = true;
		}
	}
	else if(dir ==1){ // eastbound
		if(y == 10){
			wallsensor = true;
		}
	}
	else if(dir ==2){ // southbound		
		if(x == 10){
			wallsensor = true;
		}
	}
	else if(dir ==3){ // westbound
		if(y == 1){
			wallsensor = true; 
		}
	}
	
 	return wallsensor;
}

bool dirt_sensor_percept(int x, int y)
{
	dirtsensor = false;
 
	if(grid[x][y] == 1){ // to check if the square contains dirt 
		dirtsensor = true;
	}

	return dirtsensor;
}

bool home_sensor_percept(int x, int y)
{
	homesensor = false;

	if(x == 10 and y ==1 ){ // to cheek if robot is staring point (10, 1)
		homesensor = true;
	}

	return homesensor;
}

bool right_grid_check(int x, int y, int dir)
{
	bool cango = true;

	if(dir == 0){ // northbound
		if(y==10){ // if the agent is in the rightmost 
			return cango =false;
		}
	}
	else if(dir == 1){ // eastbound
		if(x==10){ // if the agent is in the bottommost 
			return cango =false;
		}
	}
	else if(dir ==2){ // southbound
		if(y==1){ // if the agent is in the leftmost
			return cango = false;
		}
	}
	else if(dir ==3){ // westbound
		if(x==1){ // if the agent is in the topmost
			return cango = false;
		} 
	} 

	return cango;
}
 
bool right_grid_state_check(int x, int y, int dir)
{
	// to_go_right
	bool to_go_right = true;

	if(dir == 0){ // northbound
		if(cleanmemory[x][y+1]==1 || y==10 ){ // if right grid has been cleaned/end, no need to go 
			return to_go_right = false;
		}
	}
	else if(dir == 1){ // eastbound
		if(cleanmemory[x+1][y]==1 || x==10 ){ // if right grid has been cleaned/end, no need to go
			return to_go_right = false;
		}
	}
	else if(dir ==2){ // southbound
		if(cleanmemory[x][y-1]==1 || y==1 ){ // if right grid has been cleaned/end, no need to go
			return to_go_right = false;
		}
	}
	else if(dir ==3){ // westbound
		if(cleanmemory[x-1][y]==1 || x==1){ // if right grid has been cleaned/end, no need to go
			return to_go_right = false;
		} 
	}

	return to_go_right;

}

bool left_grid_check(int x, int y, int dir)
{
	bool cango = true;
	
	if(dir == 0){ // northbound
		if(y==1){ // if the agent is in the rightmost 
			return cango =false;
		}
	}
	else if(dir == 1){ // eastbound
		if(x==1){ // if the agent is in the rightmost 
			return cango =false;
		}
	}
	else if(dir ==2){ // southbound	
		if(y==10){ // if the agent is in the rightmost 
			return cango =false;
		}
	}
	else if(dir ==3){ // westbound
		if(x==10){ // if the agent is in the rightmost 
			return cango =false;
		}
	} 
	
	return cango;

}

bool left_grid_state_check(int x, int y, int dir)
{
	bool to_go_left = true;
	if(dir ==0 ){// robot is toward northbound
		if(cleanmemory[x][y-1]==1 || y==1 ){ // if left grid has been cleaned/end, no need to go 
			return to_go_left = false;
		}
	}
	else if(dir == 1){//eastbound
		if(cleanmemory[x-1][y]==1 || x==1 ){ // if left grid has been cleaned/end, no need to go 
			return to_go_left = false;
		}
	}
	else if(dir == 2){ // southbound
		if(cleanmemory[x][y+1]==1 || y==10 ){ // if left grid has been cleaned/end, no need to go 
			return to_go_left = false;
		}
	}
	else if(dir == 3){ // westbound	
		if(cleanmemory[x+1][y]==1 || x==10 ){ // if left grid has been cleaned/end, no need to go 
			return to_go_left = false;
		}
	}
	
	return to_go_left;
}

int randomized_reflex_agent()
{

	printf("INDEX, action count,go_forward(go),go_forward(could not go),turn right,turn left, suck up,  cleaned cell(0)\n");
	for (int attempt = 0; attempt < 500; attempt++) {
		initialize_variables();
		wallcnt = wall;

	//	printf("       ****************************************************************************************************************************************** \n");
		while (action_threshold--) {
			int rdaction = randomaction();
			if (rdaction == 1) {	// go forward
				int step = 0;
				if (dir_robot == 0) {
					if (step = frontgrid_togocheck(posx_robot, posy_robot, rdaction, dir_robot)) { // can go
						posx_robot -= step; //go
						actioncount++;
						goforward_go_cnt++;
					}
					else {
						goforward_notgo_cnt++;
						//	action_threshold+=1;
					}
				}
				else if (dir_robot == 1) {
					if (step = frontgrid_togocheck(posx_robot, posy_robot, rdaction, dir_robot)) { // can go
						posy_robot += step;//go
						actioncount++;
						goforward_go_cnt++;
					}
					else {
						goforward_notgo_cnt++;
						//	action_threshold+=1;
					}
				}
				else if (dir_robot == 2) {
					if (step = frontgrid_togocheck(posx_robot, posy_robot, rdaction, dir_robot)) { // can go
						posx_robot += step;//go
						actioncount++;
						goforward_go_cnt++;
					}
					else {
						goforward_notgo_cnt++;
						//action_threshold+=1;
					}
				}
				else {
					if (step = frontgrid_togocheck(posx_robot, posy_robot, rdaction, dir_robot)) { // can go
						posy_robot -= step;//go
						actioncount++;
						goforward_go_cnt++;
					}
					else {
						goforward_notgo_cnt++;
						//action_threshold+=1;
					}
				}
				//printf("action count: %d, (x:%d, y:%d), action_threshold: %d \n", actioncount, posx_robot, posy_robot, action_threshold);
			}
			if (rdaction == 2) {	// turn right
				turnright_cnt++;
				dir_robot = (dir_robot + 1) % 4;
				actioncount++;
				//printf("action count: %d, (x:%d, y:%d), action_threshold: %d \n", actioncount, posx_robot, posy_robot, action_threshold);
			}
			else if (rdaction == 3) { // turn left
				turnleft_cnt++;
				dir_robot = (dir_robot + 3) % 4;
				actioncount++;
				//printf("action count: %d, (x:%d, y:%d), action_threshold: %d \n", actioncount, posx_robot, posy_robot, action_threshold);
			}
			else if (rdaction == 4) { // suck up
				suckup_cnt++;
				grid[posx_robot][posy_robot] = 0;
				actioncount++;
				//printf("action count: %d, (x:%d, y:%d), action_threshold: %d \n", actioncount, posx_robot, posy_robot, action_threshold);
				//printf("action count:%d, suckup pos:(x:%d y:%d), dir=%d \n", actioncount, posx_robot, posy_robot, dir_robot);
			}


			if (clean_count_check() == 100 - wallcnt) {
				action = 5;
			//	printf("       ");
				for (int i = 1; i <= 10; i++) {
					for (int j = 1; j <= 10; j++) {
						//resultgrid[attempt][i][j] = grid[i][j];
						printf("%d ", grid[i][j]);
					}
					printf("\n");
				}
				action_threshold = 0;
				//	printf("action count: %d, clean cell count: %d \n", actioncount, clean_count_check() );
			}
		}


		printf("%d, %d, %d, %d, %d, %d, %d, %d \n", attempt + 1, actioncount, goforward_go_cnt, goforward_notgo_cnt, turnright_cnt, turnleft_cnt, suckup_cnt, clean_count_check());
	//	printf("     %d   %d  %d  %d   %d   %d, 'suck up':%d,  cleaned cell(0): %d \n", attempt + 1, actioncount, goforward_go_cnt, goforward_notgo_cnt, turnright_cnt, turnleft_cnt, suckup_cnt, clean_count_check());

	//	printf("       ");
		for (int i = 1; i <= 10; i++) {
			for (int j = 1; j <= 10; j++) {
				resultgrid[attempt][i][j] = grid[i][j];
			}
			//printf("\n       ");
		}
	//	printf("****************************************************************************************************************************************** \n\n");


	}
	for(int k =0; k<500; k++){
		printf("Index: %d\n",k+1);
		for(int i=1; i<=10; i++){
			for(int j=1; j<=10; j++){
				printf("%d ",resultgrid[k][i][j]);
			}
			printf("\n");
		} 
	}

	return 0;
}

int simple_deterministic_agent()
{

		printf(" INDEX,action count, 'go forward'(go), 'turn right', 'turn left', 'suck up',  cleaned cell(0)\n" );
	for (int attempt = 0; attempt < 50; attempt++) {
		initialize_variables();
		wallcnt = wall;

		//printf(" INDEX,action count, 'go forward'(go), 'turn right', 'turn left', 'suck up',  cleaned cell(0)\n" );
	//	printf("       ****************************************************************************************************************************************** \n");
		while (action_threshold--) {

			int step = 0;
			wallcnt = wall;
			if (dirt_sensor_percept(posx_robot, posy_robot)) {
				grid[posx_robot][posy_robot] = 0;
				suckup_cnt++;
				actioncount++;
			}
			else {
				if (clean_count_check() == 100) {
					action = 5;
					action_threshold = 0;
					break;
				}

				if (step = frontgrid_togocheck(posx_robot, posy_robot, 1, dir_robot)) { // can go
					int rdaction = simple_randomaction();
					if (rdaction == 1) {
						if (dir_robot == 0) {
							posx_robot -= step; //go
							actioncount++;
							goforward_go_cnt++;
						}
						else if (dir_robot == 1) {
							posy_robot += step;//go
							actioncount++;
							goforward_go_cnt++;
						}
						else if (dir_robot == 2) {
							posx_robot += step;//go
							actioncount++;
							goforward_go_cnt++;
						}
						else {
							posy_robot -= step;//go
							actioncount++;
							goforward_go_cnt++;
						}
					}
					else if (rdaction == 2) {
						turnright_cnt++;
						dir_robot = (dir_robot + 1) % 4;
						actioncount++;
					}
					else {
						turnleft_cnt++;
						dir_robot = (dir_robot + 3) % 4;
						actioncount++;
					}
				}
				else {
					int rdaction = simple_lr_randomaction();
					if (rdaction == 2) {
						turnright_cnt++;
						dir_robot = (dir_robot + 1) % 4;
						actioncount++;
					}
					else {
						turnleft_cnt++;
						dir_robot = (dir_robot + 3) % 4;
						actioncount++;
					}
				}
			}
		}
		printf("%d, %d, %d, %d, %d, %d, %d \n", attempt + 1, actioncount, goforward_go_cnt, turnright_cnt, turnleft_cnt, suckup_cnt, clean_count_check());
	//	printf("     INDEX:%d: action count: %d, 'go forward'(go):%d, 'turn right'%d, 'turn left':%d, 'suck up':%d,  cleaned cell(0): %d \n", attempt + 1, actioncount, goforward_go_cnt, turnright_cnt, turnleft_cnt, suckup_cnt, clean_count_check());

		//printf("       ");
		for (int i = 1; i <= 10; i++) {
			for (int j = 1; j <= 10; j++) {
				resultgrid[attempt][i][j] = grid[i][j];
				//printf("%d ", grid[i][j]);
			}
			//printf("\n       ");
		}
	}
	
	for(int k =0; k<50; k++){
		printf("Index: %d\n",k+1);
		for(int i=1; i<=10; i++){
			for(int j=1; j<=10; j++){
				printf("%d ",resultgrid[k][i][j]);
			}
			printf("\n");
		}
	} 
	//printf("action count: %d, clean count: %d \n", actioncount, clean_count_check() );
	return 0;
}


int model_based_deterministic_agent()
{

	struct flag f1;
	f1.a = 1;
	f1.b = 0; 
	f1.c = 0;

	printf("Index, action count, go forward(go), turn right, turn left, suck up,  cleaned cell(0)\n" );
	for(int attempt =0; attempt<50; attempt++){
		initialize_variables();
		wallcnt = wall;

	//	printf("Index, action count, go forward(go), turn right, turn left, suck up,  cleaned cell(0)\n" );
//		printf("       ****************************************************************************************************************************************** \n");
	

		while(action_threshold--){		
	
			int step = 0;
			wallcnt = wall;
		
			if ( dirt_sensor_percept(posx_robot, posy_robot) || f1.a == 1 ){ // clean the dust
				grid[posx_robot][posy_robot] = 0;
				suckup_cnt ++;
				actioncount++;
				f1.a = 0;
			//	printf( "action count: %d, suckup, (x,y):(%d, %d) \n",actioncount, posx_robot, posy_robot);
			}
			else{
				if(clean_count_check() == 100){
					action = 5;
					break;
					action_threshold = 0;
				}
				int step = frontgrid_togocheck(posx_robot, posy_robot, 1, dir_robot);
				if(f1.a == 0 && f1.b ==0 && f1.c ==0){
					if(step==0){// wall O -> turn left
						turnleft_cnt++;
						dir_robot = (dir_robot +3) %4 ;
						actioncount++; 
						f1.c = 0;
						f1.b = 0;
					}
					else{// no wall -> go forward 50%, turn left 50%
						int act  = model_action_50percent();
						//printf("act count %d, act:%d\n", actioncount, act);
						if(act ==1) {// go forward 50%
							if(dir_robot ==0){
								posx_robot -=step ; //go
								actioncount++;
								goforward_go_cnt++;
							}
							else if(dir_robot ==1 ){
								posy_robot += step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							else if(dir_robot ==2){
								posx_robot += step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							else{
								posy_robot -= step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							f1.a = grid[posx_robot][posy_robot] == 0? 0:1;
							f1.c = 0;//wall
							f1.b = 1;
						}
						else{//  turn left 50%
							turnleft_cnt++;
							dir_robot = (dir_robot +3) %4 ;
							actioncount++; 
							f1.c = 0;
							f1.b = 0;
						}
					}
				}
				else if	(f1.a ==0 && f1.b == 0 && f1.c == 1 ){
					if(step ==0){// wall O -> turn right
						turnright_cnt++;
						dir_robot = (dir_robot +3) %4 ;
						actioncount++; 
						f1.c = 1;
						f1.b = 0;
					}
					else{ // no wall -> go forward 50%, turn right 50% 
						int act  = model_action_50percent();
						if(act ==1 ){// go forward
							if(dir_robot ==0){
								posx_robot -=step ; //go
								actioncount++;
								goforward_go_cnt++;
							}
							else if(dir_robot ==1 ){
								posy_robot += step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							else if(dir_robot ==2){
								posx_robot += step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							else{
								posy_robot -= step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							f1.a = grid[posx_robot][posy_robot] == 0? 0:1;
							f1.c = 0;//wall
							f1.b = 1;
						}
						else{// right
							turnright_cnt++;
							dir_robot = (dir_robot +3) %4 ;
							actioncount++; 
							f1.c = 1;
							f1.b = 0;
						}
					}
				}
				else{
					if(step ==0){ // wall O -> turn right 50%, turn left 50%
						int act  = model_action_50percent();
						if(act ==1){ // left
							turnleft_cnt++;
							dir_robot = (dir_robot +3) %4 ;
							actioncount++; 
							f1.c = 0;
							f1.b = 0;
						}
						else{ //right
							turnright_cnt++;
							dir_robot = (dir_robot +3) %4 ;
							actioncount++; 
							f1.c = 1;
							f1.b = 0;
						}
					}
					else{ // no wall -> go forward 33.3% , turn right 33.3# , turn left 33.3#
						int act = model_action_one_third();
						//printf("act: %d \n",act);
						if(act ==1){//go forward
							if(dir_robot ==0){
								posx_robot -=step ; //go
								actioncount++;
								goforward_go_cnt++;
							}
							else if(dir_robot ==1 ){
								posy_robot += step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							else if(dir_robot ==2){
								posx_robot += step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							else{
								posy_robot -= step;//go
								actioncount++;
								goforward_go_cnt++;
							}
							f1.a = grid[posx_robot][posy_robot] == 0? 0:1;
							f1.b = 1;
							f1.c = 0;
						}
						else if(act ==2){ // left
							turnleft_cnt++;
							dir_robot = (dir_robot +3) %4 ;
							actioncount++; 
							f1.c = 0;
							f1.b = 0;
						}
						else{ //right
							turnright_cnt++;
							dir_robot = (dir_robot +3) %4 ;
							actioncount++; 
							f1.c = 1;
							f1.b = 0;
						}
					}
				}
			}	
		}
		//printf("     INDEX:%d: action count: %d, 'go forward'(go):%d, 'turn right'%d, 'turn left':%d, 'suck up':%d,  cleaned cell(0): %d \n", attempt+1,actioncount, goforward_go_cnt, turnright_cnt, turnleft_cnt, suckup_cnt,  clean_count_check() );
		printf(" %d, %d, %d, %d, %d, %d, %d \n", attempt+1,actioncount, goforward_go_cnt, turnright_cnt, turnleft_cnt, suckup_cnt,  clean_count_check() );

		printf("       ");
		for(int i=1; i<=10; i++){
			for(int j =1; j<=10; j++){
				resultgrid[attempt][i][j] = grid[i][j];
				//printf("%d ",grid[i][j]);
			}
			//printf("\n       ");
		}
	}

	for(int k =0; k<50; k++){
		printf("Index: %d\n",k+1);
		for(int i=1; i<=10; i++){
			for(int j=1; j<=10; j++){
				printf("%d ",resultgrid[k][i][j]);
			}
			printf("\n");
		} 
	}
		
	//printf("action count: %d, clean count: %d \n", actioncount, clean_count_check() );
	return 0;
}
