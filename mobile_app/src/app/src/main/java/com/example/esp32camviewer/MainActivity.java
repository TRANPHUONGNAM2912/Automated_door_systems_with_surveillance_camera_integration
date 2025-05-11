package com.example.esp32camviewer;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebResourceError;
import android.webkit.WebResourceRequest;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.content.Context;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import android.os.Handler;
import java.util.Timer;
import java.util.TimerTask;
// Firebase imports
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;

public class MainActivity extends AppCompatActivity {
    private WebView webView;
    private EditText ipAddressInput;
    private Button connectButton;
    private Button toggleControlsButton;
    private Button settingsButton;
    private Button openDoorButton;
    private Button closeDoorButton;
    private LinearLayout controlsLayout;
    private TextView timeTextView;
    private String streamUrl;
    private String settingsUrl;
    private String openDoorUrl;
    private String closeDoorUrl;
    private boolean controlsVisible = false;
    private boolean inSettingsMode = false;
    private Handler handler = new Handler();
    private Timer timer;
    
    // Firebase database reference
    private DatabaseReference doorStatusRef;
    private DatabaseReference bellStatusRef;
    private DatabaseReference warningRef;
    private DatabaseReference cameraIpRef;
    private boolean previousBellStatus = false;
    private boolean previousFingerprintStatus = false;
    private boolean previousPasswordStatus = false;
    private boolean previousRfidStatus = false;
    private boolean isReconnecting = false;
    
    // Notification channel ID
    private static final String CHANNEL_ID = "doorbell_channel";
    private static final String SECURITY_CHANNEL_ID = "security_alerts_channel";
    private static final int NOTIFICATION_ID = 100;
    private static final int FINGERPRINT_NOTIFICATION_ID = 101;
    private static final int PASSWORD_NOTIFICATION_ID = 102;
    private static final int RFID_NOTIFICATION_ID = 103;

    private Timer streamReloadTimer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Đặt ứng dụng vào chế độ toàn màn hình
        getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // Ẩn thanh điều hướng và thanh trạng thái
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
        decorView.setSystemUiVisibility(uiOptions);

        setContentView(R.layout.activity_main);

        // Tạo notification channel (yêu cầu cho Android 8.0+)
        createNotificationChannels();

        // Thiết lập Firebase database reference
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        doorStatusRef = database.getReference("door_status");
        bellStatusRef = database.getReference("bell");
        warningRef = database.getReference("warning");
        cameraIpRef = database.getReference("camera_ip");
        
        // Thiết lập lắng nghe sự thay đổi trạng thái các cảm biến
        setupBellStatusListener();
        setupSecurityListeners();
        setupCameraIpListener();

        // Khởi tạo các thành phần
        webView = findViewById(R.id.webView);
        ipAddressInput = findViewById(R.id.ipAddressInput);
        connectButton = findViewById(R.id.connectButton);
        toggleControlsButton = findViewById(R.id.toggleControlsButton);
        controlsLayout = findViewById(R.id.controlsLayout);
        timeTextView = findViewById(R.id.timeTextView);
        openDoorButton = findViewById(R.id.openDoorButton);
        closeDoorButton = findViewById(R.id.closeDoorButton);
        
        // Thêm nút Settings
        settingsButton = findViewById(R.id.settingsButton);

        // Bắt đầu cập nhật thời gian
        startTimeUpdates();

        // Cấu hình WebView
        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setUseWideViewPort(true);
        webView.getSettings().setLoadWithOverviewMode(true);
        webView.getSettings().setBuiltInZoomControls(true); // Cho phép zoom
        webView.getSettings().setDisplayZoomControls(false); // Ẩn nút zoom mặc định
        webView.setWebViewClient(new WebViewClient() {
            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                // Khi có lỗi kết nối, thử lấy IP từ Firebase và kết nối lại
                if (!isReconnecting) {
                    isReconnecting = true;
                    fetchCameraIpFromFirebase();
                }
            }
        });

        // Xử lý sự kiện khi nhấn nút kết nối
        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ipAddress = ipAddressInput.getText().toString().trim();
                if (!ipAddress.isEmpty()) {
                    // Lưu địa chỉ IP vào SharedPreferences
                    saveIpAddress(ipAddress);
                    
                    // Tạo URL stream và settings
                    setupUrls(ipAddress);
                    loadStream();
                    
                    // Ẩn controls sau khi kết nối
                    hideControls();
                } else {
                    Toast.makeText(MainActivity.this, "Vui lòng nhập địa chỉ IP", Toast.LENGTH_SHORT).show();
                }
            }
        });
        
        // Tải IP đã lưu và tự động kết nối khi khởi động
        String savedIp = getSavedIpAddress();
        if (savedIp != null && !savedIp.isEmpty()) {
            ipAddressInput.setText(savedIp);
            setupUrls(savedIp);
            loadStream();
        } else {
            // Nếu không có IP đã lưu, thử lấy từ Firebase
            fetchCameraIpFromFirebase();
        }

        // Xử lý sự kiện khi nhấn nút hiển thị/ẩn controls
        toggleControlsButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleControls();
            }
        });
        
        // Xử lý sự kiện khi nhấn nút settings
        settingsButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleSettingsMode();
            }
        });
        
        // Xử lý sự kiện khi nhấn nút mở cửa
        openDoorButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                controlDoor(true);
            }
        });
        
        // Xử lý sự kiện khi nhấn nút đóng cửa
        closeDoorButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                controlDoor(false);
            }
        });
    }
    
    private void createNotificationChannels() {
        // Tạo notification channels cho Android 8.0+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            // Doorbell channel
            CharSequence doorName = "Doorbell";
            String doorDescription = "Doorbell notification channel";
            int doorImportance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel doorChannel = new NotificationChannel(CHANNEL_ID, doorName, doorImportance);
            doorChannel.setDescription(doorDescription);
            
            // Security alerts channel
            CharSequence securityName = "Security Alerts";
            String securityDescription = "Security and authentication alerts channel";
            int securityImportance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel securityChannel = new NotificationChannel(SECURITY_CHANNEL_ID, securityName, securityImportance);
            securityChannel.setDescription(securityDescription);
            securityChannel.enableVibration(true);
            securityChannel.setVibrationPattern(new long[]{0, 1000, 500, 1000}); // Stronger vibration pattern
            
            // Đăng ký kênh với hệ thống
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(doorChannel);
                notificationManager.createNotificationChannel(securityChannel);
            }
        }
    }
    
    private void setupCameraIpListener() {
        // Lắng nghe sự thay đổi địa chỉ IP camera trong Firebase
        cameraIpRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                if (dataSnapshot.exists()) {
                    String newIpAddress = dataSnapshot.getValue(String.class);
                    if (newIpAddress != null && !newIpAddress.isEmpty()) {
                        // Cập nhật IP mới nếu khác với IP hiện tại
                        String currentIp = ipAddressInput.getText().toString().trim();
                        if (!newIpAddress.equals(currentIp)) {
                            ipAddressInput.setText(newIpAddress);
                            // Lưu địa chỉ IP mới
                            saveIpAddress(newIpAddress);
                            
                            // Nếu đang ở chế độ kết nối lại, thử kết nối với IP mới
                            if (isReconnecting) {
                                setupUrls(newIpAddress);
                                loadStream();
                                Toast.makeText(MainActivity.this, "Đang kết nối với IP mới: " + newIpAddress, Toast.LENGTH_SHORT).show();
                                isReconnecting = false;
                            }
                        }
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                Toast.makeText(MainActivity.this, 
                    "Lỗi đọc địa chỉ IP camera: " + databaseError.getMessage(), 
                    Toast.LENGTH_SHORT).show();
            }
        });
    }
    
    private void fetchCameraIpFromFirebase() {
        // Lấy địa chỉ IP camera từ Firebase ngay lập tức
        cameraIpRef.get().addOnCompleteListener(task -> {
            if (task.isSuccessful() && task.getResult() != null) {
                String firebaseIp = task.getResult().getValue(String.class);
                if (firebaseIp != null && !firebaseIp.isEmpty()) {
                    // Cập nhật IP mới
                    ipAddressInput.setText(firebaseIp);
                    saveIpAddress(firebaseIp);
                    
                    // Tạo URL và thử kết nối lại
                    setupUrls(firebaseIp);
                    loadStream();
                    
                    // Đã tắt thông báo khi cập nhật IP mới từ Firebase
                    
                    isReconnecting = false;
                } else {
                    // Đã tắt thông báo khi không thể lấy địa chỉ IP từ Firebase
                    isReconnecting = false;
                }
            } else {
                Toast.makeText(MainActivity.this, 
                    "Lỗi khi lấy địa chỉ IP từ Firebase", 
                    Toast.LENGTH_SHORT).show();
                isReconnecting = false;
            }
        });
    }
    
    private void setupBellStatusListener() {
        bellStatusRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                if (dataSnapshot.exists()) {
                    Boolean currentBellStatus = dataSnapshot.getValue(Boolean.class);
                    
                    if (currentBellStatus != null && currentBellStatus && !previousBellStatus) {
                        // Hiển thị thông báo khi bell chuyển từ false sang true
                        showBellNotification();
                    }
                    
                    // Cập nhật trạng thái trước đó
                    if (currentBellStatus != null) {
                        previousBellStatus = currentBellStatus;
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                Toast.makeText(MainActivity.this, 
                    "Lỗi đọc trạng thái chuông: " + databaseError.getMessage(), 
                    Toast.LENGTH_SHORT).show();
            }
        });
    }
    
    private void setupSecurityListeners() {
        // Theo dõi thay đổi trong nút warning/fingerprint
        warningRef.child("fingerprint").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                if (dataSnapshot.exists()) {
                    Boolean currentStatus = dataSnapshot.getValue(Boolean.class);
                    
                    if (currentStatus != null && currentStatus && !previousFingerprintStatus) {
                        // Hiển thị thông báo khi fingerprint chuyển từ false sang true
                        showSecurityNotification(
                            FINGERPRINT_NOTIFICATION_ID,
                            "Cảnh báo bảo mật",
                            "Cảnh báo nhập vân tay sai quá số lần quy định!"
                        );
                    }
                    
                    // Cập nhật trạng thái trước đó
                    if (currentStatus != null) {
                        previousFingerprintStatus = currentStatus;
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                Toast.makeText(MainActivity.this, 
                    "Lỗi đọc trạng thái vân tay: " + databaseError.getMessage(), 
                    Toast.LENGTH_SHORT).show();
            }
        });
        
        // Theo dõi thay đổi trong nút warning/password
        warningRef.child("password").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                if (dataSnapshot.exists()) {
                    Boolean currentStatus = dataSnapshot.getValue(Boolean.class);
                    
                    if (currentStatus != null && currentStatus && !previousPasswordStatus) {
                        // Hiển thị thông báo khi password chuyển từ false sang true
                        showSecurityNotification(
                            PASSWORD_NOTIFICATION_ID,
                            "Cảnh báo bảo mật",
                            "Nhập mật khẩu sai quá số lần quy định!"
                        );
                    }
                    
                    // Cập nhật trạng thái trước đó
                    if (currentStatus != null) {
                        previousPasswordStatus = currentStatus;
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                Toast.makeText(MainActivity.this, 
                    "Lỗi đọc trạng thái mật khẩu: " + databaseError.getMessage(), 
                    Toast.LENGTH_SHORT).show();
            }
        });
        
        // Theo dõi thay đổi trong nút warning/rfid
        warningRef.child("rfid").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                if (dataSnapshot.exists()) {
                    Boolean currentStatus = dataSnapshot.getValue(Boolean.class);
                    
                    if (currentStatus != null && currentStatus && !previousRfidStatus) {
                        // Hiển thị thông báo khi rfid chuyển từ false sang true
                        showSecurityNotification(
                            RFID_NOTIFICATION_ID,
                            "Cảnh báo bảo mật",
                            "Quét thẻ RFID sai quá số lần quy định!"
                        );
                    }
                    
                    // Cập nhật trạng thái trước đó
                    if (currentStatus != null) {
                        previousRfidStatus = currentStatus;
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                Toast.makeText(MainActivity.this, 
                    "Lỗi đọc trạng thái RFID: " + databaseError.getMessage(), 
                    Toast.LENGTH_SHORT).show();
            }
        });
    }
    
    private void showBellNotification() {
        // Hiển thị toast ngay lập tức để người dùng thấy ngay
        Toast.makeText(this, "Có người bấm chuông!", Toast.LENGTH_LONG).show();
        
        // Tạo intent để mở app khi nhấn vào thông báo
        Intent intent = new Intent(this, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, 
            PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT);
        
        // Tạo thông báo trong notification drawer
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_notification) // Cần thêm icon này vào thư mục drawable
                .setContentTitle("ESP32 Camera Viewer")
                .setContentText("Có người bấm chuông!")
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true)
                .setVibrate(new long[]{0, 500, 200, 500}) // Rung 2 lần
                .setTimeoutAfter(30000); // Tự động đóng sau 30 giây
        
        // Hiển thị thông báo
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        try {
            notificationManager.notify(NOTIFICATION_ID, builder.build());
        } catch (SecurityException e) {
            // Có thể thiếu quyền POST_NOTIFICATIONS trên Android 13+
            Toast.makeText(this, "Không thể hiển thị thông báo: Thiếu quyền", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
        
        // Thêm hiệu ứng chú ý (chỉ khi đang ở màn hình chính)
        if (!inSettingsMode) {
            // Flash màn hình hoặc hiệu ứng khác để thu hút sự chú ý
            flashScreen();
        }
    }
    
    private void showSecurityNotification(int notificationId, String title, String message) {
        // Hiển thị toast ngay lập tức để người dùng thấy ngay
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
        
        // Tạo intent để mở app khi nhấn vào thông báo
        Intent intent = new Intent(this, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, notificationId, intent, 
            PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT);
        
        // Tạo thông báo bảo mật trong notification drawer
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, SECURITY_CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_notification)
                .setContentTitle(title)
                .setContentText(message)
                .setPriority(NotificationCompat.PRIORITY_MAX) // Ưu tiên cao nhất
                .setCategory(NotificationCompat.CATEGORY_ALARM) // Chỉ định là cảnh báo
                .setContentIntent(pendingIntent)
                .setAutoCancel(true)
                .setVibrate(new long[]{0, 1000, 500, 1000}) // Rung mạnh hơn
                .setTimeoutAfter(60000); // Tự động đóng sau 60 giây
        
        // Hiển thị thông báo
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        try {
            notificationManager.notify(notificationId, builder.build());
        } catch (SecurityException e) {
            // Có thể thiếu quyền POST_NOTIFICATIONS trên Android 13+
            Toast.makeText(this, "Không thể hiển thị thông báo: Thiếu quyền", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
        
        // Thêm hiệu ứng chú ý với màu đỏ đậm hơn (chỉ khi đang ở màn hình chính)
        if (!inSettingsMode) {
            flashSecurityScreen();
        }
    }
    
    private void flashScreen() {
        final View rootView = getWindow().getDecorView();
        final int originalColor = rootView.getSolidColor();
        
        // Đổi màu nền sang màu đỏ
        rootView.setBackgroundColor(0x88FF0000); // Semi-transparent red
        
        // Sau 500ms đổi lại màu nền cũ
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                rootView.setBackgroundColor(originalColor);
            }
        }, 500);
    }
    
    private void flashSecurityScreen() {
        final View rootView = getWindow().getDecorView();
        final int originalColor = rootView.getSolidColor();
        
        // Đổi màu nền sang màu đỏ đậm hơn cho cảnh báo bảo mật
        rootView.setBackgroundColor(0xBFFF0000); // More opaque red
        
        // Flash hai lần để nhấn mạnh tính nghiêm trọng
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                rootView.setBackgroundColor(originalColor);
                
                // Flash lần 2
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        rootView.setBackgroundColor(0xBFFF0000);
                        
                        // Trở về màu ban đầu
                        handler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                rootView.setBackgroundColor(originalColor);
                            }
                        }, 500);
                    }
                }, 300);
            }
        }, 500);
    }
    
    private void startTimeUpdates() {
        timer = new Timer();
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        updateTimeDisplay();
                    }
                });
            }
        }, 0, 1000); // Cập nhật mỗi giây
    }
    
    private void updateTimeDisplay() {
        SimpleDateFormat sdf = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss", Locale.getDefault());
        String currentTime = sdf.format(new Date());
        timeTextView.setText(currentTime);
    }

    private void toggleControls() {
        controlsVisible = !controlsVisible;
        controlsLayout.setVisibility(controlsVisible ? View.VISIBLE : View.GONE);
    }
    
    private void hideControls() {
        controlsVisible = false;
        controlsLayout.setVisibility(View.GONE);
    }
    
    private void setupUrls(String ipAddress) {
        streamUrl = "http://" + ipAddress + ":81/stream";
        settingsUrl = "http://" + ipAddress;
        openDoorUrl = "http://" + ipAddress + "/control?door=open";
        closeDoorUrl = "http://" + ipAddress + "/control?door=close";
    }
    
    private void controlDoor(boolean open) {
        try {
            // Cập nhật trạng thái door_status trên Firebase Realtime Database
            doorStatusRef.setValue(open);
            
            String message = open ? "Đang mở cửa..." : "Đang đóng cửa...";
            Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
            
            // Hiệu ứng cho nút
            Button doorButton = open ? openDoorButton : closeDoorButton;
            animateDoorButton(doorButton);
        } catch (Exception e) {
            Toast.makeText(this, "Lỗi điều khiển cửa: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
    }
    
    private void animateDoorButton(final Button button) {
        button.setAlpha(0.5f);
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                button.setAlpha(1.0f);
            }
        }, 200);
    }

    private void loadStream() {
        try {
            inSettingsMode = false;
            webView.loadUrl(streamUrl);
            
            // Start periodic reload timer
            startStreamReloadTimer();
            
        } catch (Exception e) {
            Toast.makeText(this, "Lỗi kết nối: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
    }
    
    private void startStreamReloadTimer() {
        // Hủy timer hiện tại nếu có
        stopStreamReloadTimer();
        
        // Tạo timer mới để tự động tải lại stream mỗi 10 giây
        streamReloadTimer = new Timer();
        streamReloadTimer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (!inSettingsMode) {
                            // Chỉ tải lại nếu đang ở chế độ xem stream
                            webView.reload();
                        }
                    }
                });
            }
        }, 10000, 10000); // Tải lại mỗi 10 giây
    }
    
    private void stopStreamReloadTimer() {
        if (streamReloadTimer != null) {
            streamReloadTimer.cancel();
            streamReloadTimer = null;
        }
    }
    
    private void loadSettings() {
        try {
            inSettingsMode = true;
            webView.loadUrl(settingsUrl);
            Toast.makeText(this, "Đang mở cài đặt camera", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            Toast.makeText(this, "Lỗi kết nối cài đặt: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
    }
    
    private void toggleSettingsMode() {
        if (inSettingsMode) {
            loadStream();
        } else {
            loadSettings();
        }
    }
    
    private void saveIpAddress(String ipAddress) {
        getSharedPreferences("ESP32CAMViewer", Context.MODE_PRIVATE)
            .edit()
            .putString("saved_ip", ipAddress)
            .apply();
    }
    
    private String getSavedIpAddress() {
        return getSharedPreferences("ESP32CAMViewer", Context.MODE_PRIVATE)
            .getString("saved_ip", "");
    }

    @Override
    protected void onPause() {
        super.onPause();
        webView.onPause();
        
        // Dừng timer tải lại stream khi app không hiển thị
        stopStreamReloadTimer();
    }

    @Override
    protected void onResume() {
        super.onResume();
        webView.onResume();

        // Đảm bảo chế độ toàn màn hình được duy trì khi quay lại ứng dụng
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
        decorView.setSystemUiVisibility(uiOptions);
        
        // Khởi động lại timer tải lại stream nếu đang ở chế độ xem stream
        if (!inSettingsMode) {
            startStreamReloadTimer();
        }
    }
    
    @Override
    public void onBackPressed() {
        if (inSettingsMode) {
            loadStream();
        } else if (controlsVisible) {
            hideControls();
        } else {
            super.onBackPressed();
        }
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (timer != null) {
            timer.cancel();
        }
        stopStreamReloadTimer();
    }
}