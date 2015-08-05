#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/current_function.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using namespace std;

class MySimpleClient
{
public:
  MySimpleClient( boost::asio::io_service& ios, std::string host, std::string port)
  : _ios(ios),_socket(ios),_resolver(ios),_t(ios),_host(host),_port(port)
  {
    connect();
  }

private:
  boost::asio::io_service& _ios;
  boost::asio::ip::tcp::tcp::socket _socket;
  tcp::resolver _resolver;
  boost::asio::deadline_timer _t;
  std::string _host;
  std::string _port;
  
  std::vector<char> data = std::vector<char>(2);
  
  void reconnect_in_5_sec()
  {
    cout << BOOST_CURRENT_FUNCTION << endl;
    _t.expires_from_now(boost::posix_time::seconds(5));
    _t.async_wait(boost::bind(&MySimpleClient::connect,this));

  }
  
  void write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
  {
    cout << BOOST_CURRENT_FUNCTION << endl;
    if (!ec)
    {
    cout << "Gesendet: " << bytes_transferred << endl;
    boost::asio::async_read(_socket,boost::asio::buffer(data,2),boost::bind(&MySimpleClient::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      cout << "Error.." << ec.message() << endl;
      reconnect_in_5_sec();
    }
  }
  
  void read_handler(const boost::system::error_code &ec,  std::size_t bytes_transferred)
  {
    cout << BOOST_CURRENT_FUNCTION << endl;
    if (!ec)
    {
    cout << "Empfange !!! " << bytes_transferred << " bytes: " <<int(data[0]) << " " << int(data[1]) << endl;
    boost::asio::async_write(_socket,boost::asio::buffer(data,2),boost::bind(&MySimpleClient::write_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      cout << "Error.." << ec.message() << endl;
      reconnect_in_5_sec();
    }
  }

  void connect_handler(const boost::system::error_code& ec)
  {
    cout << BOOST_CURRENT_FUNCTION << endl;
    if(!ec)
    {
        boost::asio::async_read(_socket, boost::asio::buffer(data, 2),
        boost::bind(&MySimpleClient::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else{
      cout << "Error..." << ec.message() << endl;
      reconnect_in_5_sec();
    }
  }

  void resolve_handler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator it)
  {
    cout << BOOST_CURRENT_FUNCTION << endl;
    if(!ec)
    {
      boost::asio::async_connect(_socket,it,boost::bind(&MySimpleClient::connect_handler,this,boost::asio::placeholders::error));
    }
    else{
      cout << "Error.." << ec.message() << endl;
      reconnect_in_5_sec();
    }
  }
  
  void connect()
  {
    cout << BOOST_CURRENT_FUNCTION << endl;
    tcp::resolver::query query(tcp::v4(),_host,_port);
    _resolver.async_resolve(query, boost::bind(&MySimpleClient::resolve_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
  }
};


int main(int argc, char* argv[])
{
  cout << BOOST_CURRENT_FUNCTION << endl;
  try
  {
    if(argc != 3)
    {
      std::cerr << "Use: " << argv[0] << " <host> <port>" << std::endl;
      return -1;
    }
    boost::asio::io_service io_service;
    MySimpleClient msc(io_service,argv[1],argv[2]);
    while(true){
      io_service.run();
      usleep(1000);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
