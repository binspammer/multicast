#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

const short multicastPort = 30001;
const int maxMessageCount = 10;

class Render
{
public:
   Render(boost::asio::io_service& io_service, const boost::asio::ip::address& multicast_address)
   : endpoint_(multicast_address, multicastPort)
   , socket_(io_service, endpoint_.protocol())
   , timer_(io_service)
   , messageCount_(0)
   {
      std::ostringstream os;
      os << "Message " << messageCount_++;
      message_ = os.str();
      
      socket_.async_send_to( boost::asio::buffer(message_), endpoint_,
               boost::bind(&Render::handleSendTo, this, boost::asio::placeholders::error) );
   }
   
   void handleSendTo(const boost::system::error_code& error)
   {
      if (!error && messageCount_ < maxMessageCount)
      {
         timer_.expires_from_now(boost::posix_time::seconds(1));
         timer_.async_wait( boost::bind(&Render::handleTimeout, this, boost::asio::placeholders::error) );
      }
   }
   
   void handleTimeout(const boost::system::error_code& error)
   {
      if (!error)
      {
         std::ostringstream os;
         os << "Message " << messageCount_++;
         message_ = os.str();
         
         socket_.async_send_to( boost::asio::buffer(message_), endpoint_,
                  boost::bind(&Render::handleSendTo, this, boost::asio::placeholders::error) );
      }
   }
   
private:
   boost::asio::ip::udp::endpoint endpoint_;
   boost::asio::ip::udp::socket socket_;
   boost::asio::deadline_timer timer_;
   int messageCount_;
   std::string message_;
};

int main(int argc, char* argv[])
try
{
   if (argc != 2)
   {
      std::cerr << "Usage: sender <multicast_address>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    sender 239.255.0.1\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    sender ff31::8000:1234\n";
      return 1;
   }
   
   boost::asio::io_service ioService;
   Render s(ioService, boost::asio::ip::address::from_string(argv[1]));
   ioService.run();
   
   return 0;
}
catch (std::exception& e)
{
   std::cerr << "Exception: " << e.what() << "\n";
}

