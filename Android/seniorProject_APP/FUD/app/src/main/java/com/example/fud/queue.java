package com.example.fud;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;

public class queue extends AppCompatActivity {

    private TextView queueContainer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_queue);

        queueContainer = (TextView) findViewById(R.id.queueListContainer);

        importData();
    }

    public void importData() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                final StringBuilder builder = new StringBuilder();

                try {
                    Document doc = Jsoup.connect("http://sproject.highridgeroofing.com/index.php?Mode=3").get();
                    builder.append(doc.body());

                } catch (IOException e) {
                    builder.append("Error: ").append(e.getMessage());
                }

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        builder.replace(0, 8, "");
                        builder.replace(builder.length()-13, builder.length(), "");
                        queueContainer.setText(builder.toString().replaceAll("<br>", ""));


                    }
                });

            }
        }).start();
    }
}
