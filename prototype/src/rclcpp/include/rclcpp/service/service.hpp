#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <functional>
#include <memory>

#include <rclcpp/rclcpp.hpp>

#include <rclcpp/node/node.hpp>
#include <rclcpp/publisher/publisher.hpp>
#include <boost/shared_ptr.hpp>

#include <genidlcpp/resolver.h>

#include <ccpp_dds_dcps.h>

namespace rclcpp
{
    namespace service
    {
        template <typename ROSRequestType, typename ROSResponseType>
        class Service
        {
        public:
            typedef std::function<bool (typename ROSRequestType::ConstPtr, typename ROSResponseType::Ptr)> CallbackType;
            typedef std::shared_ptr< Service<ROSRequestType, ROSResponseType> > Ptr;


            Service(const std::string& service_name, rclcpp::node::Node *node, CallbackType cb) : service_name_(service_name), node_(node), cb_(cb) {}

            ~Service() {}

            void handle_request(typename ROSRequestType::ConstPtr req)
            {
                size_t queue_size = 0;
                std::string topic_name = this->service_name_ + "_response_" + req->client_id;
                std::cout << "Publishing server responses to topic named: " << topic_name << std::endl;
                auto publisher = this->node_->template get_publisher<ROSResponseType>(
                    topic_name, queue_size);
                typename ROSResponseType::Ptr res(new ROSResponseType());
                this->cb_(req, res);
                res->req_id = req->req_id;
                res->client_id = req->client_id;
                publisher->publish(res);
            }

        private:
            std::string service_name_;
            CallbackType cb_;
            rclcpp::node::Node *node_;

        };
    }
}
#endif
