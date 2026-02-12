#include "main.h"
#include <algorithm>
#include <cstdlib>


// Conveyor and top roller motors
inline pros::Motor conveyor(20, pros::v5::MotorGear::green);
inline pros::Motor top_roller(11, pros::v5::MotorGear::green);

// Drivetrain motor groups (match opcontrol directions)
inline pros::MotorGroup left_mg({16, 18, 17});
inline pros::MotorGroup right_mg({13, 14, 12});

// Conveyor control macros
#define conveyor_on() conveyor.move(120)
#define conveyor_off() conveyor.move(0)
#define conveyor_reverse() conveyor.move(-120)

// Top roller control macros
#define top_roller_on() top_roller.move(-120)
#define top_roller_off() top_roller.move(0)
#define top_roller_reverse() top_roller.move(90)
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
	forward(90, 50); \
	stop(); \
	turnleft(90, 90); \
	stop(); \
	forward(90, 500); \
	stop(); \
	turnleft(90, 90); \
	stop(); \
	forward(90, 50); \
	stop();\
	turnleft(90, 90);} while(0)
	
	#define traverse_match_load() do {turnright(90, 90); \
	stop(); \
	forward(90, 500); \
	stop();\
	turnleft(90, 90);} while(0)

// Match loader solenoids (ports G and H)
pros::ADIDigitalOut descorer('G');
pros::ADIDigitalOut match_loader_solenoid('H');

// Turn both on
#define intake_on() do { conveyor_on(); top_roller_on(); } while(0)

// Store match loads (conveyor on, top roller in reverse at half speed)
#define store_match_load() do { conveyor.move(120); top_roller.move(55); } while(0)

// A simple autonomous function that drives forward for a short time
void skeleton_auto() {
	//Still need first move here.

	forward(90, 300);
	turnleft(90, 500);
	stop();


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
void opcontrol() {
	
	pros::Controller master(pros::E_CONTROLLER_MASTER);

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
		static int current_throttle = 0;  // Track previous throttle value
		int raw_throttle = master.get_digital(DIGITAL_R2) - master.get_digital(DIGITAL_L2);
		int target_throttle = raw_throttle * 127;

		// Smoothly ramp to target
		if (current_throttle < target_throttle) {
			current_throttle += 8;  // Accelerate up
			if (current_throttle > target_throttle) current_throttle = target_throttle;
		} else if (current_throttle > target_throttle) {
			current_throttle -= 8;  // Decelerate/reverse smoothly
			if (current_throttle < target_throttle) current_throttle = target_throttle;
		}

		int throttle = current_throttle;
		int turn = master.get_analog(ANALOG_LEFT_X);   // Left joystick X for turning
		int conveyor_speed = master.get_analog(ANALOG_RIGHT_Y);  // Right joystick Y for conveyor
		turn /= 2;  // Reduces turn value for easier control

		if (std::abs(turn) <= 3) {
			turn = 0;  // Deadband to prevent drift when driving straight
		}

	
		int left = throttle - turn;      // Left motors: throttle minus turn
		int right = throttle + turn;     // Right motors: throttle plus turn

		int max_mag = std::max(std::abs(left), std::abs(right));
		if (max_mag > 127) {
			float scale = 127.0f / max_mag;
			left = static_cast<int>(left * scale);
			right = static_cast<int>(right * scale);
		}

		left_mg.move(left);
		right_mg.move(right);
		if (match_load_enabled) {
			store_match_load();
		} else if (shoot_enabled) {
			intake_on();
		} else {
			conveyor.move(conveyor_speed);      // Conveyor controlled by right joystick
			top_roller_off();
		}

		//B: Loading toggle - Toggle on/off
	 	if (master.get_digital_new_press(DIGITAL_B)) {
	 		match_load_enabled = !match_load_enabled;
	 		if (match_load_enabled) {
	 			shoot_enabled = false;
	 		}
	 	}

		// X: "Shoot" button - Toggle on/off
	 	if (master.get_digital_new_press(DIGITAL_X)) {
	 		shoot_enabled = !shoot_enabled;
	 		if (shoot_enabled) {
	 			match_load_enabled = false;
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
