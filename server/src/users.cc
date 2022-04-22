#include <map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <utility>

#define LWS_WITH_NETWORK
#include <libwebsockets.h>

extern "C"
{
#include "protocol.h"
#include "message.h"
}

/**
 * Required since structs (in C, at least) does not come with any operator by default.
 * Internal use only: This was meant to be used by std::map for comparision
**/
struct _Compare
{
    constexpr bool
    operator()(const pss_data& x, const pss_data& y) const
    {
        return (x.wsi == y.wsi);
    }
};

std::map<struct pss_data, struct aaruserdata, _Compare> users;

extern "C"
{

struct aaruserdata aarGetConnectedUser(struct pss_data wsi)
{
    return users.at(wsi);
}

struct aaruserdata* aarGetAllUsers()
{
    std::vector<struct aaruserdata> nuse;
    struct aaruserdata* nusecarr = (struct aaruserdata*) 
                    std::malloc(sizeof(struct aaruserdata) * users.size());

    /* Thanks C++ for forcing me to make a enourmous type. */
    for (std::map<struct pss_data, 
                            struct aaruserdata, 
                            _Compare>::iterator 
                            it = users.begin();
                            it != users.end(); ++it)
    {
        nuse.push_back(it->second);
    }

    std::copy(nuse.begin(), nuse.end(), nusecarr);
    return nusecarr;
}

struct pss_data* aarGetConnectedClients(size_t* lpSize)
{
    std::vector<struct pss_data> nuse;
    struct pss_data* nusecarr = (struct pss_data*) (
                            std::malloc(sizeof(struct pss_data) * users.size()
                        ));
    for (std::map<struct pss_data, 
                            struct aaruserdata, 
                            _Compare>::iterator 
                            it = users.begin();
                            it != users.end(); ++it)
    {
        nuse.push_back(it->first);
    }

    std::copy(nuse.begin(), nuse.end(), nusecarr);
    std::memset(lpSize, 0, sizeof(size_t));
    (*lpSize) = nuse.size();
    return nusecarr;
}

struct aaruserdata aarRemoveUser(struct pss_data user)
{
    struct aaruserdata oldU = aarGetConnectedUser(user);
    users.erase(user);
    return oldU;
}

void aarPutUser(struct pss_data wsi, struct aaruserdata user)
{
    users.insert(std::make_pair(wsi, user));
}

}
