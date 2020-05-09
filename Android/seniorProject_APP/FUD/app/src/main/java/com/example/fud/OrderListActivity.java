package com.example.fud;

import android.content.Intent;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.view.View;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;

public class OrderListActivity extends AppCompatActivity {
    public static final String EXTRA_MESSAGE = "message";
    public static String slot = "none";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_order_list);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }

    public void orderActivity(View view) {
        Intent intent = new Intent(this, SetOrderActivity.class);
        intent.putExtra(EXTRA_MESSAGE, slot);
        startActivity(intent );
    }


    public void onClickOneMeal(View view) {
        slot = "1";
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

        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);


    }

    public void onClickTwoMeal(View view) {
        slot = "2";
        final String orderAPI_URL = "http://sproject.highridgeroofing.com/index.php?Mode=2&Name=Alex&slot=2";

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


        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);

    }

}
