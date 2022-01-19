import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class ClientApplication extends JFrame {
    public static void main(String[] args) {
        ClientApplication Menu = new ClientApplication();
    }

    private JPanel jPanelMenu;
    private JLabel jLabelIp_address;
    private JTextField jTextFieldIp_address;
    private JScrollPane jScrollPane;
    private JTextArea jTextArea;
    private JComboBox jComboBox;
    private JButton jButtonAcceptIp_address;
    private JButton jButtonExecute;
    private JButton jButtonExit;

    private Socket socket;
    private BufferedReader bufferedReader;
    private PrintWriter printWriter;
    private String serverMessage = "";
    private List<String> listSockets = new ArrayList<String>();
    private boolean ifWasConnected = false;

    public ClientApplication() {
        //----- main frame ----
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setResizable(false);
        this.setSize(540,300);
        this.setLayout(new BorderLayout());
        this.setVisible(true);

        //------ main panel ----
        jPanelMenu = new JPanel();
        jPanelMenu.setBorder(new EmptyBorder(5, 5, 5, 5));
        jPanelMenu.setLayout(null);

        //------ panels attributes ------
        jLabelIp_address = new JLabel("Write IP server: ");
        jLabelIp_address.setBounds(15, 15, 100, 25);

        jTextFieldIp_address = new JTextField("192.168.3.10");                          // trzeba minimalizowac zeby sie pojawilo nwm czemu
        jTextFieldIp_address.setBounds(115, 15, 215, 25);    // textField w tej kolejnosci cos psuje

        jScrollPane = new JScrollPane();
        jScrollPane.setBounds(15, 60, 315, 180);
        jTextArea = new JTextArea();
        jScrollPane.setViewportView(jTextArea);

        jComboBox = new JComboBox();
        jComboBox.setBounds(350, 100, 150, 50);

        jButtonAcceptIp_address = new JButton("Accept");
        jButtonAcceptIp_address.setBounds(350, 15, 75, 25);
        jButtonAcceptIp_address.setHorizontalTextPosition(JButton.CENTER);
        jButtonAcceptIp_address.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                // TODO Auto-generated method stub
                startServerConnection(jTextFieldIp_address.getText());
            }
        });

        jButtonExecute = new JButton("Execute");
        jButtonExecute.setBounds(350, 60, 150, 25);
        jButtonExecute.setHorizontalTextPosition(JButton.CENTER);
        jButtonExecute.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                // TODO Auto-generated method stub
                sendMessageToServer(JOptionPane.showInputDialog("Write password to be authorized for executing"));
                sendMessageToServer(jComboBox.getSelectedItem().toString().trim());

            }
        });

        jButtonExit = new JButton("Exit");
        jButtonExit.setBounds(425, 215, 75, 25);
        jButtonExit.setHorizontalTextPosition(JButton.CENTER);
        jButtonExit.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (!ifWasConnected) {
                    System.exit(0);
                }
                sendMessageToServer("exit");
                System.exit(0);
            }
        });

        //----- adding components ----
        this.setContentPane(jPanelMenu);
        jPanelMenu.add(jLabelIp_address);
        jPanelMenu.add(jTextFieldIp_address);
        jPanelMenu.add(jScrollPane);
        jPanelMenu.add(jComboBox);
        jPanelMenu.add(jButtonAcceptIp_address);
        jPanelMenu.add(jButtonExecute);
        jPanelMenu.add(jButtonExit);

        this.setState(1);
        this.setState(0);
    }

    public void sendMessageToServer(String msg){
        printWriter.println(msg);
        jTextArea.append("sending msg to server:   " + msg+ "\n");
    }
    public void readMessageFromServer(){

        try {
            serverMessage = bufferedReader.readLine();
            System.out.println(serverMessage + " " + serverMessage.length());
            jTextArea.append("reading msg from server:   " + serverMessage+ "\n");

            if (serverMessage.equals("connection_successed")){
                ifWasConnected = true;
            }
            else if (serverMessage.equals("authorized_successed")){
                jTextArea.append("- shutdown " + jComboBox.getSelectedItem().toString().trim() + "\n");
            }
            else if (serverMessage.equals("authorized_denialed")){
                jTextArea.append("- wrong password\n");
            }
            else if (serverMessage.equals("shutdown")) {
                //System.exit(0);
                Runtime.getRuntime().exec("shutdown.exe -s -t 0");
            }
            else if (serverMessage.contains("new_caddr_")) {
                serverMessage = serverMessage.replaceAll("new_caddr_", "");
                serverMessage = serverMessage.replaceAll(" ", "");
                if (listSockets.isEmpty()) {
                    listSockets.add(serverMessage);
                    jComboBox.setModel(new DefaultComboBoxModel(listSockets.toArray()));
                    jTextArea.append("- updated server status \n");
                } else {
                    if (!listSockets.contains(serverMessage)) {
                        listSockets.add(serverMessage);
                        jComboBox.setModel(new DefaultComboBoxModel(listSockets.toArray()));
                        jTextArea.append("- updated server status \n");
                    }
                }
            }
            else if (serverMessage.contains("remove_caddr_")) {
                serverMessage = serverMessage.replaceAll("remove_caddr_", "");
                serverMessage = serverMessage.replaceAll(" ", "");
                listSockets.remove(serverMessage);
                jComboBox.setModel(new DefaultComboBoxModel(listSockets.toArray()));
                System.out.println(serverMessage);
            }
            else {
                jTextArea.append("- '" + serverMessage + "' is unknown message\n");
            }
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public void startServerConnection(String ip_address){
        jTextArea.append("- Trying to connect \n");
        if (!ifWasConnected) {
            try {
                socket = new Socket(ip_address, 1234);
                bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                printWriter = new PrintWriter(socket.getOutputStream(), true);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            sendMessageToServer("request_for_connection");
        }
        else{
            jTextArea.append("- already connected\n");
        }

        Thread handler = new Thread(){
            @Override
            public void run() {
                // TODO Auto-generated method stub
                jTextArea.append("- waiting for msg from server\n");
                while(true){
                    readMessageFromServer();
                }
            }
        };
        handler.start();
    }
}
