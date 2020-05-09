package com.example.fud;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        BottomNavigationView navView = findViewById(R.id.nav_view);
        AppBarConfiguration appBarConfiguration = new AppBarConfiguration.Builder(
                R.id.navigation_home, R.id.navigation_dashboard, R.id.navigation_notifications)
                .build();
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        NavigationUI.setupActionBarWithNavController(this, navController, appBarConfiguration);
        NavigationUI.setupWithNavController(navView, navController);

    }



    public void orderActivity(View view) {
        Intent intent = new Intent(this, OrderListActivity.class);
        startActivity(intent);
    }

    public void delayedActivity(View view) {
        Intent intent = new Intent(this, DelayedOrderActivity.class);
        startActivity(intent);
    }

    public void cancelActivity(View view) {
        Intent intent = new Intent(this, CancelOrderActivity.class);
        startActivity(intent);
    }

    public void queueActivity(View view) {
        Intent intent = new Intent(this, queue.class);
        startActivity(intent);
    }

}
