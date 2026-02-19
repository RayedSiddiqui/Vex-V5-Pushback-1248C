#include "main.h"

// Conveyor and top roller motors
inline pros::Motor conveyor(20, pros::v5::MotorGear::green);
inline pros::Motor top_roller(11, pros::v5::MotorGear::green);

pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::MotorGroup left_mg({-16, 18, 17});    // Creates a motor group with forwards ports 17 & 18 and reversed port 2
	pros::MotorGroup right_mg({-13, -14, 12});  // Creates a motor group with forwards port 5 and reversed ports 4 & 6

// Match loader solenoids (ports G and H)
pros::ADIDigitalOut descorer('G');
pros::ADIDigitalOut match_loader_solenoid('H');


// Conveyor control macros
#define conveyor_on() conveyor.move(120)
#define conveyor_off() conveyor.move(0)
#define conveyor_reverse() conveyor.move(-120)

// Top roller control macros
#define top_roller_on() top_roller.move(-120)
#define top_roller_off() top_roller.move(0)
#define top_roller_reverse() top_roller.move(90)


// Turn both on
#define intake_on() do { conveyor_on(); top_roller_on(); } while(0)

// Store match loads (conveyor on, top roller in reverse at half speed)
#define store_match_load() do { conveyor.move(120); top_roller.move(55); } while(0)

// autonomous movement helper + macros
#define drive_ms(l, r, ms) do { left_mg.move(l); right_mg.move(r); pros::delay(ms); left_mg.move(0); right_mg.move(0); } while(0)
#define turnright(speed, ms) drive_ms((speed), -(speed), (ms))
#define turnleft(speed, ms) drive_ms(-(speed), (speed), (ms))
#define forward(speed, ms) drive_ms((speed), (speed), (ms))
#define backward(speed, ms) drive_ms(-(speed), -(speed), (ms))
#define stop() do { left_mg.move(0); right_mg.move(0); pros::delay(50); } while(0)
#define score() do { top_roller_on(); pros::delay(300); top_roller_off(); } while(0)
#define jiggle() do { left_mg.move(-50); right_mg.move(-50); pros::delay(100); left_mg.move(50); right_mg.move(50); pros::delay(100); } while(0)
#define lower_match_loader() do { match_loader_solenoid.set_value(true); pros::delay(100); } while(0)
#define raise_match_loader() do { match_loader_solenoid.set_value(false); pros::delay(100); } while(0)

#define load_score() do { \
		top_roller_reverse(); \
		conveyor_on(); \
		backward(90, 200); \
		stop();	\
		lower_match_loader(); \
		backward(90, 100);\
		stop();\
		pros::delay(500); \
		jiggle(); \
		jiggle(); \
		forward(90, 100); \
		stop(); \
		raise_match_loader(); \
		forward(90, 200); \
		stop(); \
		score(); \
		backward(90, 50); \
		stop(); } while(0)

#define traverse_long_goal() do {turnright(90, 90); \
		forward(100, 50); \
		stop(); \
		turnleft(100, 90); \
		stop(); \
		forward(100, 500); \
		stop(); \
		turnleft(100, 90); \
		stop(); \
		forward(100, 50); \
		stop();\
		turnleft(100, 90);} while(0)
	
#define traverse_match_load() do {turnright(90, 90); \
		stop(); \
		forward(90, 500); \
		stop();\
		turnleft(90, 90);} while(0)



// Turn both on
#define intake_on() do { conveyor_on(); top_roller_on(); } while(0)

// Store match loads (conveyor on, top roller in reverse at half speed)
#define store_match_load() do { conveyor.move(120); top_roller.move(55); } while(0)

void skeleton_auto() {
	//start right, matchload and score 6 balls and use descore 
	// move through middle and score bottom
	// go to left, matchload score 6 balls and use descore
	//go park

	//step 1
	/*
	drive straight
	turn 90 degrees cw
	match load - jiggle jiggle
	drive backwards 
	score 
	drive forward
	turn 90 degrees cw
	drive forward
	turn 90 degrees + some more ccw 
	drive backward and hope aligner lines up the descore
	put the descore down and drag balls into a middle 
	pick up descore 
	drive forward
	*/

	//step 2
	/*
	turn 90 degrees + some cw
	go into the mid balls on right  side by driving straight
	turn 45 degrees ccw 
	drive into mid balls on left  side by driving straight
	turn 45 degrees + or - some more ccw to look at top 
	drive backwards into it and drop them
	*/

	//step 3
	/*
	drive straight
	turn 45 degrees ccw
	match load - jiggle jiggle
	drive backwards 
	score 
	drive forward
	turn 90 degrees ccw
	drive forward
	turn 90 degrees + some more ccw 
	drive forward and hope aligner lines up the descore
	put the descore down and drag balls into a middle 
	pick up descore 
	*/

	//step 4
	/*
	come straight backward 
	turn 90 degrees cw
	drive straight into park and done 
	*/

	// STEP 1: Right side match load and score
	forward(-93, 500);
	turnleft(180, 180);
	load_score();
	// forward(90, 300);
	// stop();
	// turnright(90, 90);
	// stop();
	// forward(90, 300);
	// stop();
	// turnleft(90, 180);
	// stop();
	// descorer.set_value(true);
	// backward(90, 400);
	// stop();
	// pros::delay(200);
	// descorer.set_value(false);
	// forward(90, 400);
	// stop();

	// // STEP 2: Middle balls
	// turnright(90, 100);
	// stop();
	// forward(90, 600);
	// stop();
	// turnleft(45, 60);
	// stop();
	// forward(90, 400);
	// stop();
	// turnleft(45, 80);
	// stop();
	// backward(90, 300);
	// stop();
	// score();
	// stop();

	// // STEP 3: Left side match load and score
	// forward(90, 300);
	// stop();
	// turnleft(45, 60);
	// stop();
	// load_score();
	// forward(90, 300);
	// stop();
	// turnleft(90, 90);
	// stop();
	// forward(90, 300);
	// stop();
	// turnleft(90, 180);
	// stop();
	// descorer.set_value(true);
	// forward(90, 400);
	// stop();
	// pros::delay(200);
	// descorer.set_value(false);
	// backward(90, 400);
	// stop();

	// // STEP 4: Park
	// backward(90, 600);
	// stop();
	// turnright(90, 90);
	// stop();
	// forward(90, 400);
	// stop();

}


/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Rayed FTW");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	skeleton_auto();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void opcontrol() {
	skeleton_auto();
}

