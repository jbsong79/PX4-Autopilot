/*
   Temporary Test
   Currently testing : PX4_simple_app with duplicate folder
*/

#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>
#include <poll.h>
//Using message channels between application using uORB Topic
#include <uORB/topics/sensor_combined.h>

extern "C" __EXPORT int MyNewTest_main(int argc, char *argv[]);


int MyNewTest_main(int argc, char *argv[])
{
	static int16_t exit_iteration = 0;
	PX4_INFO("Hello Sky!");
	int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
	/* limit the update rate to 5 Hz */
	orb_set_interval(sensor_sub_fd, 200);
	/* one could wait for multiple topics with this technique, just using one here */
	px4_pollfd_struct_t fds[] = {
	{ .fd = sensor_sub_fd,   .events = POLLIN },
	};
	while (true) {
		/* wait for sensor update of 1 file descriptor for 1000 ms (1 second) */
		int poll_ret = px4_poll(fds, 1, 1000);
		if (poll_ret == 0) {
			/* this means none of our providers is giving us data */
			PX4_ERR("Got no data within a second");

		}
		if (fds[0].revents & POLLIN) {
			/* obtained data for the first file descriptor */
			struct sensor_combined_s raw;
			/* copy sensors raw data into local buffer */
			orb_copy(ORB_ID(sensor_combined), sensor_sub_fd, &raw);
			PX4_INFO("Accelerometer:\t%8.4f\t%8.4f\t%8.4f",
				(double)raw.accelerometer_m_s2[0],
				(double)raw.accelerometer_m_s2[1],
				(double)raw.accelerometer_m_s2[2]);
		}
		exit_iteration++;
		if(exit_iteration>=100) {
			PX4_INFO("Exiting");
			return OK;
		}
	}
	return OK;
}
