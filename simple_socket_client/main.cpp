#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/current_function.hpp>

using boost::asio::ip::tcp;
using namespace std;

tcp::socket* so;

std::array<char, 200> bytes;

void write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);


void read_handler(const boost::system::error_code &ec,  std::size_t bytes_transferred)
{
  cout << BOOST_CURRENT_FUNCTION << endl;
  if (!ec)
  {
  cout << "Empfange !!! " << bytes_transferred << " bytes: " <<int(bytes[0]) << " " << int(bytes[1]) << endl;
  boost::asio::async_write(*so,boost::asio::buffer(bytes,2),write_handler);
  }
  else
  {
    cout << "Error.." << ec.message() << endl;
  }
}

void write_handler(const boost::system::error_code &ec,
  std::size_t bytes_transferred)
{
  cout << BOOST_CURRENT_FUNCTION << endl;
  if (!ec)
  {
  cout << "Gesendet: " << bytes_transferred << endl;
  boost::asio::async_read(*so,boost::asio::buffer(bytes,2),read_handler);
  }
  else
  {
    cout << "Error.." << ec.message() << endl;
  }
}

int main(int argc, char* argv[])
{
  cout << BOOST_CURRENT_FUNCTION << endl;

  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: argv[0] <host> <port>\n";
      return 1;
    }



    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::socket s(io_service);
    so=&s;
    boost::asio::connect(s, iterator);

    boost::asio::async_read(s,boost::asio::buffer(bytes,2),read_handler);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
