package com.example.fud;

import android.content.Intent;
import android.os.Bundle;

import com.example.fud.ui.home.HomeFragment;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.view.View;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;

public class SetOrderActivity extends AppCompatActivity {

    private int slot;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_set_order);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }


    public void orderActivity(View view) {
        pushOrder();
        updateProfileHistory();

    }

    private void updateProfileHistory() {
        /*
        final String orderAPI_URL = "http://sproject.highridgeroofing.com/index.php?Mode=2&Name=Alex&slot=1";

        new Thread(new Runnable() {
            @Override
            public void run() {
                final StringBuilder builder = new StringBuilder();

                try {
                    Document doc = Jsoup.connect(orderAPI_URL).post();
                    builder.append(doc.body());

                } catch ( IOException e) {
                    builder.append( "Error: ").append( e.getMessage() );
                }

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {



                    }
                });

            }
        } ).start();*/
    }

    private void pushOrder() {

        final String orderAPI_URL = "http://sproject.highridgeroofing.com/index.php?Mode=2&Name=Alex&slot=1";

        new Thread(new Runnable() {
            @Override
            public void run() {
                final StringBuilder builder = new StringBuilder();

                try {
                    Document doc = Jsoup.connect(orderAPI_URL).post();
                    builder.append(doc.body());

                } catch ( IOException e) {
                    builder.append( "Error: ").append( e.getMessage() );
                }

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {



                    }
                });

            }
        } ).start();
    }

}
