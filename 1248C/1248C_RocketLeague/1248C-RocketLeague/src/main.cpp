#include "main.h"

// Conveyor and top roller motors
inline pros::Motor conveyor(20, pros::v5::MotorGear::green);
inline pros::Motor top_roller(11, pros::v5::MotorGear::green);

// Conveyor control macros
#define conveyor_on() conveyor.move(120)
#define conveyor_off() conveyor.move(0)
#define conveyor_reverse() conveyor.move(-120)

// Top roller control macros
#define top_roller_on() top_roller.move(-120)
#define top_roller_off() top_roller.move(0)
#define top_roller_reverse() top_roller.move(90)

// Match loader solenoids (ports G and H)
pros::ADIDigitalOut descorer('G');
pros::ADIDigitalOut match_loader_solenoid('H');

// Turn both on
#define intake_on() do { conveyor_on(); top_roller_on(); } while(0)

// Store match loads (conveyor on, top roller in reverse at half speed)
#define store_match_load() do { conveyor.move(120); top_roller.move(55); } while(0)

// A simple autonomous function that drives forward for a short time
void dummy_auto() {
	pros::MotorGroup left_mg({-16, 18, -17});
	pros::MotorGroup right_mg({-13, 14, -12});

	// Drive for 100ms to approximate 2 inches
	top_roller_reverse();
	conveyor_on();
	left_mg.move(-100);
	right_mg.move(-100);
	pros::delay(300);  
	left_mg.move(0);
	right_mg.move(0);
	top_roller_off();
	conveyor_off();
	
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
	
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::MotorGroup left_mg({-16, 18, 17});    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::MotorGroup right_mg({-13, -14, 12});  // Creates a motor group with forwards port 5 and reversed ports 4 & 6

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

	 	// Rocket League driving control scheme
		int raw_throttle = master.get_digital(DIGITAL_R2) - master.get_digital(DIGITAL_L2);
		int throttle = raw_throttle * 127;  // Scale to motor range
		int turn = master.get_analog(ANALOG_LEFT_X);   // Left joystick X for turning
		int conveyor_speed = master.get_analog(ANALOG_RIGHT_Y);  // Right joystick Y for conveyor

		turn = turn * 0.5;  // Reduces turn value for easier control
		left_mg.move(throttle - turn);      // Left motors: throttle minus turn
		right_mg.move(throttle + turn);     // Right motors: throttle plus turn
		conveyor.move(conveyor_speed);      // Conveyor controlled by right joystick 

		//A: Loading toggle - Toggle on/off
	 	if (master.get_digital_new_press(DIGITAL_A)) {
	 		match_load_enabled = !match_load_enabled;
	 		if (match_load_enabled)
	 			store_match_load();
	 		else {
	 			conveyor_off();
	 			top_roller_off();
	 		}
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
		
		// R1: Toggle match loader down/up
		if (master.get_digital_new_press(DIGITAL_R1)) {
			match_loader_solenoid_enable = !match_loader_solenoid_enable;
			match_loader_solenoid.set_value(match_loader_solenoid_enable);
		}

		// L1: Descorer down/up
		if (master.get_digital_new_press(DIGITAL_L1)) {
			descorer_enabled = !descorer_enabled;
			descorer.set_value(descorer_enabled);
		}
	 	pros::delay(20);  // Run for 20 ms then update
	 }
}
