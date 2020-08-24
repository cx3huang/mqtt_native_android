# MQTT App for Android
Done for a PoC and exercise to use JNI and link native libraries. If needed, use Java Paho MQTT library for Android.
## Building libraries
	git clone --branch v1.3.5 https://github.com/eclipse/paho.mqtt.c.git paho-mqtt
	cd paho-mqtt
	cmake -Bbld/armeabi-v7a -H. \
   		-GNinja \
   		-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
   		-DANDROID_ABI=armeabi-v7a \
   		-DANDROID_NATIVE_API_LEVEL=24 \
   		-DPAHO_BUILD_DOCUMENTATION=FALSE \
   		-DPAHO_BUILD_STATIC=TRUE \
   		-DPAHO_BUILD_SHARED=FALSE \
   		-DCMAKE_BUILD_TYPE=Release \
   		-DPAHO_ENABLE_TESTING=FALSE \
   		-DPAHO_WITH_SSL=TRUE \
	&& cmake --build bld/armeabi-v7a

## Linking Paho MQTT C library to Android Studio
Add Paho MQTT library directory to local.properties file. Then, take a path variable from this file:  
### build.gradle
Load in the library directory value to a path variable.
	
	Properties properties = new Properties()
	properties.load(project.rootProject.file('local.properties').newDataInputStream())
	def paho_sdk_path = properties.getProperty('paho.dir')
The rest in android { defaultConfig { 
	
	...
	ndk {
		abiFilters 'armeabi-v7a'
		moduleName "paho-mqtt-c-example"
		ldLibs "log"
	}
	
	externalNativeBuild {
		cmake {
			arguments "-DPATH_TO_PAHO:STRING=${paho_sdk_path}", '-DANDROID_TOOLCHAIN=clang', '-DANDROID_ARM_NEON=TRUE', '-DANDROID_STL=c++_static'
			cFlags '-O3', '-fsigned-char'
			cppFlags "-fexceptions", '-fsigned-char', "-I${paho_sdk_path}"
		}
	}
	...
### CMakeLists.txt
	...
	add_library( paho-mqtt-c-example SHARED test.cpp )
	...
	target_link_libraries( paho-mqtt-c-example ${PATH_TO_PAHO}/libpaho-mqtt3c.a log android OpenSLES )
	...
	
