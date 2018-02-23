/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

package io.multy.android_run_tests;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    static {
        if (BuildConfig.DEBUG) {
            System.loadLibrary("multy_core_jnid");
        } else {
            System.loadLibrary("multy_core_jni");

        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        try {
            TextView lib_version = (TextView) findViewById(R.id.lib_version);
            lib_version.setText(version());
            // Example of a call to a native method
            TextView tv = (TextView) findViewById(R.id.sample_text);
            tv.setText("In progress");
            if (runTest() == 0) {
                tv.setText("All tests OK.");
            } else {
                tv.setText("Some tests FAILED. Check logs.");
            }
        } catch (Exception e){
            e.printStackTrace();
            Toast.makeText(this, "Error " + e.getMessage(), Toast.LENGTH_LONG).show();
        }

    }

    public native int runTest();
    public native String version();
}
