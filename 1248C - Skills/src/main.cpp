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


// CONSTANTS ----------------------------------------------------------

const double WHEEL_DIAMETER = 3.25;  // inches
const double GEAR_RATIO = 1.0;       // external gear ratio (motor shaft to wheel)
const double TRACK_WIDTH = 9.5;      // inches between left and right wheels
const double WHEEL_CIRCUMFERENCE = WHEEL_DIAMETER * M_PI;


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

// Solenoid control macros
#define lower_match_loader() do { match_loader_solenoid.set_value(true); } while(0)
#define raise_match_loader() do { match_loader_solenoid.set_value(false); } while(0)


// MOVEMENT FUNCTIONS -------------------------------------------------

/**
 * Drive straight for a specified distance
 * @param inches Distance to travel (positive = forward, negative = backward)
 * @param speed Motor speed (-127 to 127)
 */
void drive_distance(double inches, int speed) {
	double target_degrees = (inches / WHEEL_CIRCUMFERENCE) * 360.0 * GEAR_RATIO;
	
	left_mg.tare_position();
	right_mg.tare_position();
	
	// Set brake mode to reduce overshoot
	left_mg.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	right_mg.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	
	int direction = (inches >= 0) ? 1 : -1;
	left_mg.move(speed * direction);
	right_mg.move(speed * direction);
	
	while (fabs(left_mg.get_position()) < fabs(target_degrees)) {
		pros::delay(10);
	}
	
	// Stop with active braking
	left_mg.move(0);
	right_mg.move(0);
	left_mg.brake();
	right_mg.brake();
}

/**
 * Turn the robot by a specified angle
 * @param degrees Angle to turn (positive = right, negative = left)
 * @param speed Motor speed (0 to 127)
 */
void turn_degrees(double degrees, int speed) {
	double arc_length = (fabs(degrees) / 360.0) * M_PI * TRACK_WIDTH;
	double target_degrees = (arc_length / WHEEL_CIRCUMFERENCE) * 360.0 * GEAR_RATIO;
	
	left_mg.tare_position();
	right_mg.tare_position();
	
	// Set brake mode to reduce overshoot
	left_mg.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	right_mg.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	
	if (degrees > 0) {
		left_mg.move(speed);
		right_mg.move(-speed);
	} else {
		left_mg.move(-speed);
		right_mg.move(speed);
	}
	
	while (fabs(left_mg.get_position()) < fabs(target_degrees)) {
		pros::delay(10);
	}
	
	// Stop with active braking
	left_mg.move(0);
	right_mg.move(0);
	left_mg.brake();
	right_mg.brake();
}

/**
 * Stop and wait
 * @param ms Milliseconds to wait
 */
void stop_wait(int ms) {
	left_mg.move(0);
	right_mg.move(0);
	pros::delay(ms);
}

/**
 * Jiggle back and forth to load rings
 */
void jiggle_load() {
	left_mg.move(-50);
	right_mg.move(-50);
	pros::delay(200);
	left_mg.move(50);
	right_mg.move(50);
	pros::delay(200);
}


// AUTONOMOUS ROUTINES ------------------------------------------------

/** 
 * Simple autonomous for testing
 */
void dummy_auto() {
	roller_reverse();
	conveyor_on();
	drive_distance(-2, 100);
	roller_off();
	conveyor_off();
}

/** 
 * Skills autonomous
 */
void skills_auto() {
	// STEP 1: Right side match load and score
	drive_distance(29.1, 75);  // Drive to match load station
	stop_wait(600);
	
	turn_degrees(87.4, 100);   // Turn to face match loader
	stop_wait(600);

	lower_match_loader();
	store();
	stop_wait(1000);
	
	drive_distance(7, 60);   // Move closer to match loader
	stop_wait(1000);
	
	// Jiggle to load rings
	for (int i = 0; i < 2; i++) {
		drive_distance(2, 50);
		stop_wait(1000);
		drive_distance(-1, 50);
	}

	drive_distance(-5, 75);  // Back away from match loader
	raise_match_loader();
	stop_wait(600);
	
	turn_degrees(-8.67,100);    // Adjust angle for scoring
	stop_wait(600);

	drive_distance(-22, 75);  // Back away from match loader

	stop_wait(600);
	
	score();                 // Start scoring
	stop_wait(2000);
	shutdown();              // Stop scoring
	
	drive_distance(2, 75);   // Move forward slightly
	stop_wait(5000);
	
	// TODO: Uncomment and tune these steps after testing STEP 1
	
	// // STEP 2: Middle balls
	// descorer.set_value(false);
	// drive_distance(8, 90);
	// stop_wait(200);
	// turn_degrees(90, 100);
	// drive_distance(12, 90);
	// turn_degrees(-45, 60);
	// drive_distance(8, 90);
	// turn_degrees(-45, 80);
	// drive_distance(-6, 90);
	// score();
	// stop_wait(500);

	// // STEP 3: Left side match load and score
	// drive_distance(6, 90);
	// turn_degrees(-45, 60);
	// drive_distance(3, 75);
	// for (int i = 0; i < 5; i++) {
	//     drive_distance(2, 75);
	//     stop_wait(500);
	//     drive_distance(-1, 75);
	// }
	// drive_distance(-5, 75);
	// score();
	// stop_wait(3000);
	// drive_distance(6, 90);
	// turn_degrees(-90, 90);
	// drive_distance(6, 90);
	// turn_degrees(-90, 180);
	// descorer.set_value(true);
	// drive_distance(8, 90);
	// stop_wait(200);
	// descorer.set_value(false);
	// drive_distance(-8, 90);

	// // STEP 4: Park
	// drive_distance(-12, 90);
	// turn_degrees(90, 90);
	// drive_distance(8, 90);
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