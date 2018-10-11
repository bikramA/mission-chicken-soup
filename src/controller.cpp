#include <atomic>
#include <ctime>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/AggregateFilter.h"

#include "madara/utility/Utility.h"
#include "madara/utility/EpochEnforcer.h"

namespace utility = madara::utility;
namespace logger = madara::logger;
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;

// default transport settings
std::string host("");
const std::string default_multicast("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;

// duration to run the simulation
double test_time(30);

// Maximum unit size of a bone piece in soup ingredients
size_t max_bone_size(1000);

// Rate at which ingredients are poured into the mission
double send_hertz(5.0);

// Total number of ingredients poured into the mission
size_t num_ingredients(100);

// handle command line arguments
void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::UDP;
      }
      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.write_domain = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-e" || arg1 == "--threads")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_threads;
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;

        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if (arg1 == "--num-ingredients")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> num_ingredients;
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer(argv[i + 1]);
        buffer >> drop_rate;

        settings.update_drop_rate(
            drop_rate, madara::transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    }
    else if (arg1 == "-q" || arg1 == "--queue-length")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.queue_length;
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "--send-hz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> send_hertz;
      }

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> test_time;
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--read-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    }
    else if (arg1 == "--zmq" || arg1 == "--0mq")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::ZMQ;
      }
      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Computes soup conversion ratio in a chicken soup mission. Requires 2 processes. The result "
          "of\n"
          "  running these processes should be that each agent\n"
          "  reports soup ingredients and soup conversion ratio.\n\n"
          " [-a|--no-latency]        do not test for latency (throughput "
          "only)\n"
          " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n"
          " [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          " [-e|--threads threads]   number of read threads\n"
          " [-f|--logfile file]      log to a file\n"
          " [-i|--id id]             the id of this agent (should be "
          "non-negative)\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          " [-m|--multicast ip:port] the multicast ip to send and listen to\n"
          " [--num-ingredients ingredients]        the number of ingredients for the soup\n"
          " [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          " [-q|--queue-length len   the buffer size to use for the test\n"
          " [-r|--reduced]           use the reduced message header\n"
          " [--send-hz hertz]        hertz to send at\n"
          " [-t|--time time]         time to burst messages for throughput "
          "test\n"
          " [-u|--udp ip:port]       the udp ips to send to (first is self to "
          "bind to)\n"
          " [-z|--read-hertz hertz]  read thread hertz speed\n"
          " [--zmq|--0mq proto://ip:port] a ZeroMQ endpoint to connect to.\n"
          "                          examples include tcp://127.0.0.1:30000\n"
          "                          or any of the other endpoint types like\n"
          "                          pgm://. For tcp, remember that the first\n"
          "                          endpoint defined must be your own, the\n"
          "                          one you are binding to, and all other\n"
          "                          agent endpoints must also be defined or\n"
          "                          no messages will ever be sent to them.\n"
          "                          Similarly, all agents will have to have\n"
          "                          this endpoint added to their list or\n"
          "                          this karl agent will not see them.\n"
          "\n",
          argv[0]);

      exit(0);
    }
  }
}

class ChickenSoupFilter : public madara::filters::AggregateFilter
{
public:
  ChickenSoupFilter() : received(0), soup_elements(0), soup_ratio(0) {}

  void filter(knowledge::KnowledgeMap&,
      const transport::TransportContext&,
      knowledge::Variables& vars)
  {
    std::stringstream buffer;
    buffer << "ingredient_size";
    buffer << received;
    uint64_t recieved_data = vars.get(buffer.str()).to_integer ();

    if (recieved_data < 0.1 * max_bone_size)
    {
      ++soup_elements;
    }
    ++received;

    if (received > 0)
    {
      soup_ratio = (double) soup_elements / received;
    }

  }

  std::atomic<uint64_t> received;
  std::atomic<uint64_t> soup_elements;
  std::atomic<double> soup_ratio;
};

int main(int argc, char** argv)
{
  // initialize settings
  settings.type = transport::MULTICAST;
  settings.queue_length = 1000000;

  // parse the user command line arguments
  handle_arguments(argc, argv);

  // setup default transport as multicast
  if (settings.hosts.size() == 0)
  {
    settings.hosts.push_back(default_multicast);
  }

  // id == 0 ? "publisher" : "subscriber"
  if (settings.id == 0)
  {
    if (num_ingredients < 1)
    {
      num_ingredients = 1;
    }

    // setup a knowledge base
    knowledge::KnowledgeBase kb(host, settings);

    // use current time as seed for random generator
    std::srand(std::time(nullptr)); 

    std::vector<knowledge::VariableReference> chicken_soup_ingredients;
    knowledge::VariableReference num_ingredients_ref = kb.get_ref("num_ingredients");

    // get a handle to the data
    for (size_t i = 0; i < num_ingredients; ++i)
    {
      std::stringstream buffer;
      buffer << "ingredient_size";
      buffer << i;
      chicken_soup_ingredients.push_back(kb.get_ref(buffer.str()));
      
      // contents that go into the chicken_soup_ingredients
      uint64_t broth_ingredients = max_bone_size * std::rand() / RAND_MAX;
      
      // if broth_ingredients are 70% of max_bone size
      if (broth_ingredients < 0.7 * max_bone_size)
        // chicken soup controller will break them to soup by the time reciever gets it
        broth_ingredients = 0.09 * max_bone_size;

      // write broth_ingredients in the knowledgebase
      kb.set(chicken_soup_ingredients[i], broth_ingredients, knowledge::EvalSettings::DELAY_NO_EXPAND);
      kb.save_context ("recent_context.kkb");
    }

    kb.set(num_ingredients_ref, num_ingredients, knowledge::EvalSettings::DELAY_NO_EXPAND);
    kb.save_context ("recent_context.kkb");

    std::cerr << "Publishing soup packets in " << num_ingredients
              << " variables @" << send_hertz << " hz for " << test_time
              << " s on " << transport::types_to_string(settings.type)
              << " transport\n";

    // use epoch enforcer"
    utility::EpochEnforcer<utility::Clock> enforcer(1 / send_hertz, test_time);

    kb.set(num_ingredients_ref, num_ingredients, knowledge::EvalSettings::SEND_NO_EXPAND);
    kb.save_context ("recent_context.kkb");

    while (!enforcer.is_done())
    {
      for (const auto& soup_ingredient : chicken_soup_ingredients)
      {
        kb.mark_modified(soup_ingredient);
      }

      kb.mark_modified(num_ingredients_ref);

      kb.send_modifieds();

      if (send_hertz > 0.0)
      {
        enforcer.sleep_until_next();
      }
    }

    kb.set(num_ingredients_ref, num_ingredients, knowledge::EvalSettings::SEND_NO_EXPAND);

    // update context to file
    kb.save_context ("recent_context.kkb");

    std::cerr << "Publisher is done. Check results on subscriber.\n";

    // load knowledge from context of past session
    kb.load_context ("recent_context.kkb", false);

    // display context from past session
    kb.print_knowledge();
  }  // end publisher
  else
  {
    // add a receive filter to keep track of latency
    ChickenSoupFilter ChickenSoupFilter;

    // check filter to extract the soup data
    settings.add_receive_filter(&ChickenSoupFilter);

    // setup a knowledge base
    knowledge::KnowledgeBase kb(host, settings);

    std::cerr << "Receiving for " << test_time << " s on "
              << transport::types_to_string(settings.type) << " transport\n";

    // subscriber lives the hard life (TM)
    utility::sleep(test_time);

    // update context to file
    kb.save_context ("recent_context.kkb");

    // stop the knowledge base from receiving anything else
    kb.close_transport();

    // print stats
    if (ChickenSoupFilter.received > 0)
    {
      uint64_t received = ChickenSoupFilter.received;
      double soup_ratio = ChickenSoupFilter.soup_ratio;

      std::cerr << "Test: SUCCESS\n";
      std::cerr << "Settings:\n";
      std::cerr << "  Transport type: "
                << transport::types_to_string(settings.type) << "\n";
      std::cerr << "  Hosts: \n";

      for (auto host : settings.hosts)
      {
        std::cerr << "    " << host << "\n";
      }

      std::cerr << "  Soup Contents recieved: " << received << " \n";
      std::cerr << "  Soup Conversion Ratio: " << soup_ratio << " \n";
    }
    // end print stats
    else
    {
      std::cerr << "Subscriber received no data.\nTest: FAIL.\n";
      return -1;
    }

    // finally update the latest context to file
    kb.save_context ("recent_context.kkb");

    // load knowledge from context of past session
    kb.load_context ("recent_context.kkb", false);

    // display context from past session 
    // uncomment to see the saved knowledge
    // kb.print_knowledge();

  } // end subscriber
 
  return 0;
}
