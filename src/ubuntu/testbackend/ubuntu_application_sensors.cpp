/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Martin Pitt <martin.pitti@ubuntu.com>
 */

#include <ubuntu/application/sensors/ubuntu_application_sensors.h>
#include <ubuntu/application/sensors/accelerometer.h>
#include <ubuntu/application/sensors/proximity.h>
#include <ubuntu/application/sensors/light.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

/***************************************
 *
 * test sensor implementation
 *
 ***************************************/

class TestSensor
{
  public:
    TestSensor(ubuntu_sensor_type _type, float _min_value, float _max_value, float _resolution) :
        type(_type),
        enabled(false),
        resolution(_resolution),
        min_delay(0),
        min_value(_min_value),
        max_value(_max_value),
        on_event_cb(NULL),
        event_cb_context(NULL),
        x(0.0),
        y(0.0),
        z(0.0),
        distance((UASProximityDistance) 0),  // LP#1256969
        timestamp(0)
    {}

  public:  /* consenting adults -- this is only internal API */
    ubuntu_sensor_type type;
    bool enabled;
    float resolution;
    uint32_t min_delay;
    float min_value, max_value;
    void (*on_event_cb)(void*, void*);
    void* event_cb_context;

    /* current value; note that we do not track separate Event objects/pointers
     * at all, and just always deliver the current value */
    float x, y, z;
    UASProximityDistance distance;
    uint64_t timestamp;
};

/* Singleton which reads the sensor data file and maintains the TestSensor
 * instances */
class SensorController
{
  public:
    // Ensure that controller is initialized, and return singleton
    static SensorController* instance()
    {
        if (_inst == NULL) {
            _inst = new SensorController();
        }
        return _inst;
    }

    /* Ensure that controller is initialized; return TestSensor of given type,
     * or NULL if it doesn't exist.
     */
    static TestSensor* get(ubuntu_sensor_type type)
    {
        // ensure that we are initialized
        instance();

        try {
            return sensors.at(type);
        } catch (const out_of_range&) {
            return NULL;
        }
    }

  private:
    SensorController();
    bool next_command();
    bool process_create_command();

    static ubuntu_sensor_type type_from_name(const string& type)
    {
        if (type == "light")
            return ubuntu_sensor_type_light;
        if (type == "proximity")
            return ubuntu_sensor_type_proximity;
        if (type == "accel")
            return ubuntu_sensor_type_accelerometer;

        cerr << "TestSensor ERROR: unknown sensor type " << type << endl;
        abort();
    }

    static SensorController* _inst;
    static map<ubuntu_sensor_type, TestSensor*> sensors;
    ifstream data;
    string current_command;
};

map<ubuntu_sensor_type, TestSensor*> SensorController::sensors;
SensorController* SensorController::_inst = NULL;

SensorController::SensorController()
{
    const char* path = getenv("UBUNTU_PLATFORM_API_SENSOR_TEST");

    if (path == NULL) {
        cerr << "TestSensor ERROR: Need $UBUNTU_PLATFORM_API_SENSOR_TEST to point to a data file\n";
        abort();
    }

    data.open(path);
    if (!data.is_open()) {
        cerr << "TestSensor ERROR: Failed to open data file " << path << ": " << strerror(errno) << endl;
        abort();
    }

    // process all "create" commands
    while (next_command()) {
        //cout << "SensorController init: command '" << current_command << "'\n";
        if (!process_create_command())
            break;
    }
}

bool
SensorController::next_command()
{
    while (getline(data, current_command)) {
        // trim leading and trailing space
        current_command.erase(0, current_command.find_first_not_of(" \t"));
        current_command.erase(current_command.find_last_not_of(" \t") + 1);
        // ignore empty or comment lines
        if (current_command.size() == 0 || current_command[0] == '#')
            continue;
        return true;
    }
    return false;
}

bool
SensorController::process_create_command()
{
    stringstream ss(current_command, ios_base::in);
    string token;

    // we only process "create" commands here; if we have something else, stop
    ss >> token;
    if (token != "create")
        return false;

    ss >> token;
    ubuntu_sensor_type type = type_from_name(token);

    if (sensors.find(type) != sensors.end()) {
        cerr << "TestSensor ERROR: duplicate creation of sensor type " << token << endl;
        abort();
    }

    float min = 0, max = 0, resolution = 0;

    if (type != ubuntu_sensor_type_proximity) {
        // read min, max, resolution
        ss >> min >> max >> resolution;

        if (max <= min) {
            cerr << "TestSensor ERROR: max_value must be >= min_value in  " << current_command << endl;
            abort();
        }
        if (resolution <= 0) {
            cerr << "TestSensor ERROR: resolution must be > 0 in " << current_command << endl;
            abort();
        }
    }

    //cout << "SensorController::process_create_command: type " << type << " min " << min << " max " << max << " res " << resolution << endl;

    sensors[type] = new TestSensor(type, min, max, resolution);
    return true;
}


/***************************************
 *
 * Acceleration API
 *
 ***************************************/

UASensorsAccelerometer* ua_sensors_accelerometer_new()
{
    return SensorController::get(ubuntu_sensor_type_accelerometer);
}

UStatus ua_sensors_accelerometer_enable(UASensorsAccelerometer* s)
{
    static_cast<TestSensor*>(s)->enabled = true;
    return (UStatus) 0;
}

UStatus ua_sensors_accelerometer_disable(UASensorsAccelerometer* s)
{
    static_cast<TestSensor*>(s)->enabled = false;
    return (UStatus) 0;
}

uint32_t ua_sensors_accelerometer_get_min_delay(UASensorsAccelerometer* s)
{
    return static_cast<TestSensor*>(s)->min_delay;
}

float ua_sensors_accelerometer_get_min_value(UASensorsAccelerometer* s)
{
    return static_cast<TestSensor*>(s)->min_value;
}

float ua_sensors_accelerometer_get_max_value(UASensorsAccelerometer* s)
{
    return static_cast<TestSensor*>(s)->max_value;
}

float ua_sensors_accelerometer_get_resolution(UASensorsAccelerometer* s)
{
    return static_cast<TestSensor*>(s)->resolution;
}

void ua_sensors_accelerometer_set_reading_cb(UASensorsAccelerometer* s, on_accelerometer_event_cb cb, void* ctx)
{
    TestSensor* sensor = static_cast<TestSensor*>(s);
    sensor->on_event_cb = cb;
    sensor->event_cb_context = ctx;
}

uint64_t uas_accelerometer_event_get_timestamp(UASAccelerometerEvent* e)
{
    return static_cast<TestSensor*>(e)->timestamp;
}

float uas_accelerometer_event_get_acceleration_x(UASAccelerometerEvent* e)
{
    return static_cast<TestSensor*>(e)->x;
}

float uas_accelerometer_event_get_acceleration_y(UASAccelerometerEvent* e)
{
    return static_cast<TestSensor*>(e)->y;
}

float uas_accelerometer_event_get_acceleration_z(UASAccelerometerEvent* e)
{
    return static_cast<TestSensor*>(e)->z;
}

/***************************************
 *
 * Proximity API
 *
 ***************************************/

UASensorsProximity* ua_sensors_proximity_new()
{
    return SensorController::get(ubuntu_sensor_type_proximity);
}

UStatus ua_sensors_proximity_enable(UASensorsProximity* s)
{
    static_cast<TestSensor*>(s)->enabled = true;
    return (UStatus) 0;
}

UStatus ua_sensors_proximity_disable(UASensorsProximity* s)
{
    static_cast<TestSensor*>(s)->enabled = false;
    return (UStatus) 0;
}

uint32_t ua_sensors_proximity_get_min_delay(UASensorsProximity* s)
{
    return static_cast<TestSensor*>(s)->min_delay;
}

// the next three function make no sense in the API, just return zero
float ua_sensors_proximity_get_min_value(UASensorsProximity*)
{
    return 0.0;
}

float ua_sensors_proximity_get_max_value(UASensorsProximity*)
{
    return 0.0;
}

float ua_sensors_proximity_get_resolution(UASensorsProximity*)
{
    return 0.0;
}

void ua_sensors_proximity_set_reading_cb(UASensorsProximity* s, on_proximity_event_cb cb, void* ctx)
{
    TestSensor* sensor = static_cast<TestSensor*>(s);
    sensor->on_event_cb = cb;
    sensor->event_cb_context = ctx;
}

uint64_t uas_proximity_event_get_timestamp(UASProximityEvent* e)
{
    return static_cast<TestSensor*>(e)->timestamp;
}

UASProximityDistance uas_proximity_event_get_distance(UASProximityEvent* e)
{
    return static_cast<TestSensor*>(e)->distance;
}


/***************************************
 *
 * Light API
 *
 ***************************************/

UASensorsLight* ua_sensors_light_new()
{
    return SensorController::get(ubuntu_sensor_type_light);
}

UStatus ua_sensors_light_enable(UASensorsLight* s)
{
    static_cast<TestSensor*>(s)->enabled = true;
    return (UStatus) 0;
}

UStatus ua_sensors_light_disable(UASensorsLight* s)
{
    static_cast<TestSensor*>(s)->enabled = false;
    return (UStatus) 0;
}

uint32_t ua_sensors_light_get_min_delay(UASensorsLight* s)
{
    return static_cast<TestSensor*>(s)->min_delay;
}

float ua_sensors_light_get_min_value(UASensorsLight* s)
{
    return static_cast<TestSensor*>(s)->min_value;
}

float ua_sensors_light_get_max_value(UASensorsLight* s)
{
    return static_cast<TestSensor*>(s)->max_value;
}

float ua_sensors_light_get_resolution(UASensorsLight* s)
{
    return static_cast<TestSensor*>(s)->resolution;
}

void ua_sensors_light_set_reading_cb(UASensorsLight* s, on_light_event_cb cb, void* ctx)
{
    TestSensor* sensor = static_cast<TestSensor*>(s);
    sensor->on_event_cb = cb;
    sensor->event_cb_context = ctx;
}

uint64_t uas_light_event_get_timestamp(UASLightEvent* e)
{
    return static_cast<TestSensor*>(e)->timestamp;
}

float uas_light_event_get_light(UASLightEvent* e)
{
    return static_cast<TestSensor*>(e)->x;
}
