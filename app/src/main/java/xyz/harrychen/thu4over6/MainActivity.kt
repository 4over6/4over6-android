package xyz.harrychen.thu4over6

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_main.*
import xyz.harrychen.thu4over6.bean.ServerInfo
import xyz.harrychen.thu4over6.bean.Traffic
import xyz.harrychen.thu4over6.bean.VpnInfo
import xyz.harrychen.thu4over6.net.VpnService4Over6
import java.lang.Exception
import java.lang.NumberFormatException
import java.net.Inet6Address
import java.net.UnknownHostException

class MainActivity : AppCompatActivity() {

    private lateinit var service: VpnService4Over6
    private lateinit var info: VpnInfo
    private lateinit var traffic: Traffic

    private var socketConnected = false
    private var vpnConnected = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        button_reset.setOnClickListener {
            input_server_addr.text.clear()
            input_server_port.text.clear()
            text_info.text = "Not Connected"
        }
        button_connect.setOnClickListener { toggleConnectState() }
    }

    private fun toggleConnectState() {

        if (vpnConnected) {
            socketConnected = false
            vpnConnected = false
            text_info.text = "Disconnected"
            toggleAllControls(true)
            button_connect.text = "Connect"
            return
        }

        val addr = input_server_addr.text.toString()
        val port = input_server_port.text.toString()
        if (!isValidIpv6Address(addr)) {
            showToast("Invalid server addr")
            return
        }
        if (!isValidPort(port)) {
            showToast("Invalid server port")
            return
        }

        toggleAllControls(false)

        text_info.text = "Connecting to $addr:$port..."
        Observable.just(establishConnection(ServerInfo(addr, port)))
            .map {
                if (!it) throw Exception()
                else requestConfiguration()!!
            }
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe({
                info = it
                // hardcode some information
                info.ipv6 = addr
                info.searchDomain = "tsinghua.edu.cn"
                socketConnected = true
                text_info.text = "Preparing to establish VPN connection:\n$info"
                service = VpnService4Over6(info)
                // TODO: establish VPN connection

                // after success
                vpnConnected = true
                button_connect.text = "Disconnect"
                button_connect.isEnabled = true
            }, {
                showToast("Error connecting to server")
                text_info.text = "Error establishing connection or requesting configuration"
                toggleAllControls(true)
            })
    }


    private fun toggleAllControls(enabled: Boolean) {
        button_connect.isEnabled = enabled
        button_reset.isEnabled = enabled
        input_server_addr.isEnabled = enabled
        input_server_port.isEnabled = enabled
    }


    private external fun establishConnection(info: ServerInfo): Boolean
    private external fun requestConfiguration(): VpnInfo?


    private fun showToast(t: String) {
        Toast.makeText(this, t, Toast.LENGTH_SHORT).show()
    }


    private fun isValidIpv6Address(addr: String): Boolean {
        // do not check in Java code
        return !addr.isBlank()
    }


    private fun isValidPort(port: String): Boolean {
        return try {
            Integer.valueOf(port) in 1..65535
        } catch (e: NumberFormatException) {
            false
        }
    }


    companion object {
        init {
            System.loadLibrary("4over6")
        }
    }
}
