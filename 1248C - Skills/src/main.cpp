#include "main.h"

// DEFINITIONS --------------------------------------------------------

// Controller
inline pros::Controller master(pros::E_CONTROLLER_MASTER);

// Conveyor and top roller motors
inline pros::Motor conveyor(20, pros::v5::MotorGear::green);
inline pros::Motor roller(11, pros::v5::MotorGear::green);

// Drivetrain motor groups
inline pros::MotorGroup left_mg({16, -18, -17});
inline pros::MotorGroup right_mg({13, 14, -12});

// Match loader solenoids (ports G and H)
inline pros::ADIDigitalOut descorer('G');
inline pros::ADIDigitalOut match_loader_solenoid('H');


// MACROS -------------------------------------------------------------

// Conveyor control macros
#define conveyor_on() conveyor.move(120)
#define conveyor_reverse() conveyor.move(-120)
#define conveyor_off() conveyor.move(0)

// Roller control macros
#define roller_on() roller.move(-120)
#define roller_reverse() roller.move(90)
#define roller_off() roller.move(0)

// Turn conveyor and roller on
#define score() do { conveyor_on(); roller_on(); } while(0)

// Reverse conveyor and roller
#define outtake() do { conveyor_reverse(); roller_reverse(); } while(0)

// Turn conveyor and roller off
#define shutdown() do { conveyor_off(); roller_off(); } while(0)

// Store match loads (conveyor on, roller in reverse at half speed)
#define store() do { conveyor.move(120); roller.move(55); } while(0)

// Autonomous movement helper + macros
#define drive_ms(l, r, ms) do { left_mg.move(l); right_mg.move(r); pros::delay(ms); left_mg.move(0); right_mg.move(0); } while(0)
#define turnright(speed, ms) drive_ms((speed), -(speed), (ms))
#define turnleft(speed, ms) drive_ms(-(speed), (speed), (ms))
#define forward(speed, ms) drive_ms((speed), (speed), (ms))
#define backward(speed, ms) drive_ms(-(speed), -(speed), (ms))
#define stop(ms) do { left_mg.move(0); right_mg.move(0); pros::delay(ms); } while(0)
#define jiggle() do { left_mg.move(-50); right_mg.move(-50); pros::delay(200); left_mg.move(50); right_mg.move(50); pros::delay(200); } while(0)
#define lower_match_loader() do { match_loader_solenoid.set_value(true); } while(0)
#define raise_match_loader() do { match_loader_solenoid.set_value(false); } while(0)


// NEW METHODS --------------------------------------------------------

/** 
 * A simple autonomous function that drives forward for a short time
 */
void dummy_auto() {
	// Drive for 100ms to approximate 2 inches
	roller_reverse();
	conveyor_on();
	left_mg.move(-100);
	right_mg.move(-100);
	pros::delay(300);  
	left_mg.move(0);
	right_mg.move(0);
	roller_off();
	conveyor_off();
}

/** 
 * Skills autonomous
 */
void skills_auto() {
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
	forward(75, 540);
	pros::delay(200);
	turnright(100, 220);
	pros::delay(200);

	lower_match_loader();
	store();
	pros::delay(1000);
	forward(75, 220);
	pros::delay(500);
	// Jiggle
	for (int i = 0; i < 5; i++) {
		forward(75, 200);
		pros::delay(500);
		backward(75, 100);
	}

	backward(75, 375);
	pros::delay(200);
	score();
	pros::delay(3000);
	forward(75, 150);

	pros::delay(5000);
	
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


// VEX METHODS --------------------------------------------------------

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
	pros::lcd::set_text(1, "Chaos is Order");

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
	dummy_auto();
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
	// Temp skills auto
	skills_auto();
	
	// State variables for controls
	bool r1_pressed = false;
	bool l1_pressed = false;
	bool r2_pressed = false;
	bool l2_pressed = false;

	// State variables for macros
	bool scoring = false;
	bool outtaking = false;
	bool storing = false;

	// State variables for solenoids
	bool match_loader_solenoid_enabled = false;
	// bool descorer_solenoid_enabled = false;

	// State variable for forward direction
	bool intake_forward = true;

	// Solenoid setting
	match_loader_solenoid.set_value(false);
	descorer.set_value(true);

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
	 	                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
	 	                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control
		// Left joystick forward/backward
		int dir = master.get_analog(ANALOG_LEFT_Y);

		// Right joystick right/left
		int turn = master.get_analog(ANALOG_RIGHT_X);
		turn = turn * 0.5;

		// Set voltage
		if (intake_forward) {
			left_mg.move(dir + turn);
			right_mg.move(dir - turn);
		} else {
			left_mg.move(-1*dir + turn);
			right_mg.move(-1*dir - turn);
		}
		

		// HIGHEST PRIORITY -- Reversing
		// R2: Conveyor outtake
		if (master.get_digital(DIGITAL_R2)) {
			r2_pressed = true;
		} else {
			r2_pressed = false;
		}

		// R2: Roller outtake
		if (master.get_digital(DIGITAL_L2)) {
			l2_pressed = true;
		} else {
			l2_pressed = false;
		}

		// Priority 2 -- Everything else
		// R1: Conveyor intake
		if (master.get_digital_new_press(DIGITAL_R1)) {
			r1_pressed = !r1_pressed;
		}

		// L1: Roller intake
		if (master.get_digital_new_press(DIGITAL_L1)) {
			l1_pressed = !l1_pressed;
		}

		// X: Scoring macro
		if (master.get_digital_new_press(DIGITAL_X)) {
			scoring = !scoring;

			if (scoring) {
				outtaking = false;
				storing = false;

				r1_pressed = false;
				l1_pressed = false;
			}
		}

		// A: Outtaking macro
		if (master.get_digital_new_press(DIGITAL_A)) {
			outtaking = !outtaking;

			if (outtaking) {
				scoring = false;
				storing = false;

				r1_pressed = false;
				l1_pressed = false;
			}
		}

		// B: Storing macro
		if (master.get_digital_new_press(DIGITAL_B)) {
			storing = !storing;

			if (storing) {
				scoring = false;
				outtaking = false;

				r1_pressed = false;
				l1_pressed = false;
			}
		}

		// LEFT: Clear macros
		if (master.get_digital_new_press(DIGITAL_LEFT)) {
			scoring = false;
			outtaking = false;
			storing = false;
		}


		// Determine which way to move conveyor and roller
		// Set booleans
		bool conveyor_fwd = false;
		bool conveyor_rev = false;
		bool roller_fwd = false;
		bool roller_rev = false;

		bool override_conveyor = false;
		bool override_roller = false;

		// Conveyor controls
		if (r2_pressed) {
			conveyor_rev = true;
			override_conveyor = true;
		} else if (r1_pressed) {
			conveyor_fwd = true;
			override_conveyor = true;
		}

		// Roller controls
		if (l2_pressed) {
			roller_rev = true;
			override_roller = true;
		} else if (l1_pressed) {
			roller_fwd = true;
			override_roller = true;
		}

		// Scoring macro
		if (scoring) {
			if (!override_conveyor) {
				conveyor_fwd = true;
			}
			if (!override_roller) {
				roller_fwd = true;
			}
		}

		// Outtaking macro
		if (outtaking) {
			if (!override_conveyor) {
				conveyor_rev = true;
			}
			if (!override_roller) {
				roller_rev = true;
			}
		}

		// Storing macro
		if (storing) {
			if (!override_conveyor) {
				conveyor_fwd = true;
			}
			if (!override_roller) {
				roller_rev = true;
			}
		}

		// Move conveyor
		if (conveyor_fwd) {
			conveyor_on();
		} else if (conveyor_rev) {
			conveyor_reverse();
		} else {
			conveyor_off();
		}

		// Move roller
		if (roller_fwd) {
			roller_on();
		} else if (roller_rev) {
			if (storing) {
				roller.move(55);
			} else {
				roller_reverse();
			}
		} else {
			roller_off();
		}

		
		// Solenoids
		// Up: Match loader down/up (toggle)
		if (master.get_digital_new_press(DIGITAL_UP)) {
			match_loader_solenoid_enabled = !match_loader_solenoid_enabled;
			match_loader_solenoid.set_value(match_loader_solenoid_enabled);
		}

		// Down: Descorer down/up (hold down)
		if (master.get_digital(DIGITAL_DOWN)) {
			descorer.set_value(false);
		} else {
			descorer.set_value(true);
		}


		// Drivetrain
		// Right: Change forward direction (toggle)
		if (master.get_digital_new_press(DIGITAL_RIGHT)) {
			intake_forward = !intake_forward;
		}


		// Run for 20 ms then update
	 	pros::delay(20);  
	 }
}