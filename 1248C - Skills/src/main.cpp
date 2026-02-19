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
	turn 90 degrees + some more cw 
	drive forward and hope aligner lines up the descore
	put the descore down and drag balls into a middle 
	pick up descore 
	*/

	//step 2
	/*
	turn 90 degrees ccw
	go into the mid balls on right  side by driving straight
	drive into mid balls on left  side by driving straight
	turn 45 degrees + or - some more ccw to look at top 
	drive backwards into it and drop them - jiggle jiggle 
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


	load_score(); //First Load

	traverse_long_goal();
	 
	load_score(); //Second Load and Score


	traverse_match_load();
	
	load_score();  //Third Load and Score
	
	traverse_long_goal();
	
	load_score();  //Fourth Load and Score

	turnright(90, 90);
	stop();
	forward(90, 100);
	stop();
	turnright(90, 90);
	stop();
	forward(90, 200);

	
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

/*void opcontrol() {
	
	
	// State variables for toggles
	bool conveyor_enabled = false;
	bool roller_enabled = false;
	bool match_load_enabled = false;
	bool match_loader_solenoid_enable = false;
	bool shoot_enabled = false;
	bool descorer_enabled = false;

	match_loader_solenoid.set_value(false);
	descorer.set_value(false);

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
	 	                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
	 	                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

	 	// Arcade control scheme
	 	int dir = master.get_analog(ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		int turn = master.get_analog(ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		turn = turn * 0.5;
		left_mg.move(dir - turn);                      // Sets left motor voltage
		right_mg.move(dir + turn);                     // Sets right motor voltage

	 	// R1: Toggle conveyor on/off
	 	if (master.get_digital_new_press(DIGITAL_R1)) {
	 		conveyor_enabled = !conveyor_enabled;
	 		if (conveyor_enabled)
	 			conveyor_on();
	 		else
	 			conveyor_off();
	 	}

	 	// L1: Toggle roller on/off
	 	if (master.get_digital_new_press(DIGITAL_L1)) {
	 		roller_enabled = !roller_enabled;
	 		if (roller_enabled)
	 			top_roller_on();
	 		else
	 			top_roller_off();
	 	}

	 	// R2: Reverse conveyor and turn off
	 	if (master.get_digital_new_press(DIGITAL_R2)) {
	 		conveyor_reverse();
	 		pros::delay(200);  // Reverse for 200ms
	 		conveyor_off();
	 		conveyor_enabled = false;
	 	}

	 	// L2: Reverse roller and turn off
	 	if (master.get_digital_new_press(DIGITAL_L2)) {
	 		top_roller_reverse();
	 		pros::delay(200);  // Reverse for 200ms
	 		top_roller_off();
	 		roller_enabled = false;
	 	}

		// X: "Shoot" button - Toggle on/off
	 	if (master.get_digital_new_press(DIGITAL_X)) {
	 		shoot_enabled = !shoot_enabled;
	 		if (shoot_enabled)
	 			intake_on();
	 		else {
	 			conveyor_off();
	 			top_roller_off();
	 		}
		}

	 	// B: Toggle loader (store_match_load) on/off
	 	if (master.get_digital_new_press(DIGITAL_B)) {
	 		match_load_enabled = !match_load_enabled;
	 		if (match_load_enabled)
	 			store_match_load();
	 		else {
	 			conveyor_off();
	 			top_roller_off();
	 		}
	 	}
		
		// A: Toggle match loader down/up
		if (master.get_digital_new_press(DIGITAL_A)) {
			match_loader_solenoid_enable = !match_loader_solenoid_enable;
			match_loader_solenoid.set_value(match_loader_solenoid_enable);
		}

		// Y: Descorer down/up
		if (master.get_digital_new_press(DIGITAL_Y)) {
			descorer_enabled = !descorer_enabled;
			descorer.set_value(descorer_enabled);
		}
	 	pros::delay(20);  // Run for 20 ms then update
	 }
}

*/