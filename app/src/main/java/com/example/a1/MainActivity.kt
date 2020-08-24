package com.example.a1

import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.runBlocking

class MainActivity : AppCompatActivity() {
    private var resultCode: Int = 0
    private lateinit var connectButton: Button
    private lateinit var subscribeButton: Button
    private lateinit var publishButton: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Example of a call to a native method
        // sample_text.text = stringFromJNI()
        /*extern "C" JNIEXPORT jstring JNICALL
        Java_com_example_a1_MainActivity_stringFromJNI(
            JNIEnv* env,
            jobject /* this */) {
            std::string hello = "Hello from C++";
            return env->NewStringUTF(hello.c_str());
        }*/

        connectButton = findViewById(R.id.connectButton)
        subscribeButton = findViewById(R.id.subscribeButton)
        publishButton = findViewById(R.id.publishButton )

        connectButton.setOnClickListener {
            connectHelper()
        }
        subscribeButton.setOnClickListener {
            publishButton.visibility = View.GONE

            val deferredMessage: Deferred<String> = GlobalScope.async {
                subscribe()
            }
            runBlocking {
                val message = deferredMessage.await()
                Toast.makeText(applicationContext, message, Toast.LENGTH_LONG).show()
                disconnect()
            }
        }
        publishButton.setOnClickListener {
            subscribeButton.visibility = View.GONE

            runBlocking {
                publish()
                Toast.makeText(applicationContext, "Publish finished", Toast.LENGTH_LONG).show()
                disconnect()
            }
        }
    }

    private fun connectHelper() {
        var count = 10
        do {
            resultCode = connect()
            if (resultCode != 0) {
                Log.e("Connection", "Connection unsuccessful")
                Toast.makeText(applicationContext, "Connection unsuccessful", Toast.LENGTH_LONG)
                    .show()
            } else
                Toast.makeText(applicationContext, "Connection successful", Toast.LENGTH_LONG)
                    .show()
            count--
        } while (resultCode != 0 && count > 0)
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    // external fun stringFromJNI(): String

    private external fun connect(): Int

    private external suspend fun subscribe(): String

    private external suspend fun publish(): Void

    private external fun disconnect(): Void

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("paho-mqtt-c-example")
        }
    }
}
