#include "http_response.h"

using namespace std;

const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css "},
    {".js", "text/javascript "},
};

const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
};

const unordered_map<int, string> HttpResponse::CODE_PATH = {
    {400, "/400.html"},
    {403, "/403.html"},
    {404, "/404.html"},
};

HttpResponse::HttpResponse()
{
    code_ = -1;
    path_ = src_dir_ = "";
    is_keep_alive_ = false;
    mm_file_ = nullptr;
    mm_file_stat_ = {0};
};

HttpResponse::~HttpResponse()
{
    UnmapFile();
}

void HttpResponse::Init(const string &src_dir, string &path, bool is_keep_alive, int code)
{
    assert(src_dir != "");
    if (mm_file_)
    {
        UnmapFile();
    }
    code_ = code;
    is_keep_alive_ = is_keep_alive;
    path_ = path;
    src_dir_ = src_dir;
    mm_file_ = nullptr;
    mm_file_stat_ = {0};
}

void HttpResponse::MakeResponse(Buffer &buff)
{
    /* 判断请求的资源文件 */
    if (stat((src_dir_ + path_).c_str(), &mm_file_stat_) < 0 || S_ISDIR(mm_file_stat_.st_mode))
    {
        code_ = 404;
    }
    else if (!(mm_file_stat_.st_mode & S_IROTH))
    {
        code_ = 403;
    }
    else if (code_ == -1)
    {
        code_ = 200;
    }
    ErrorHtml_();
    AddStateLine_(buff);
    AddHeader_(buff);
    AddContent_(buff);
}

char *HttpResponse::File()
{
    return mm_file_;
}

size_t HttpResponse::FileLen() const
{
    return mm_file_stat_.st_size;
}

void HttpResponse::ErrorHtml_()
{
    if (CODE_PATH.count(code_) == 1)
    {
        path_ = CODE_PATH.find(code_)->second;
        stat((src_dir_ + path_).c_str(), &mm_file_stat_);
    }
}

void HttpResponse::AddStateLine_(Buffer &buff)
{
    string status;
    if (CODE_STATUS.count(code_) == 1)
    {
        status = CODE_STATUS.find(code_)->second;
    }
    else
    {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buff.Append("HTTP/1.1 " + to_string(code_) + " " + status + "\r\n");
}

void HttpResponse::AddHeader_(Buffer &buff)
{
    buff.Append("Connection: ");
    if (is_keep_alive_)
    {
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6, timeout=120\r\n");
    }
    else
    {
        buff.Append("close\r\n");
    }
    buff.Append("Content-type: " + GetFileType_() + "\r\n");
}

void HttpResponse::AddContent_(Buffer &buff)
{
    int srcFd = open((src_dir_ + path_).c_str(), O_RDONLY);
    if (srcFd < 0)
    {
        ErrorContent(buff, "File NotFound!");
        return;
    }

    /* 将文件映射到内存提高文件的访问速度 
        MAP_PRIVATE 建立一个写入时拷贝的私有映射*/
    LOG_DEBUG("file path %s", (src_dir_ + path_).c_str());
    int *mm_ret = (int *)mmap(0, mm_file_stat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if (*mm_ret == -1)
    {
        ErrorContent(buff, "File NotFound!");
        return;
    }
    mm_file_ = (char *)mm_ret;
    close(srcFd);
    buff.Append("Content-length: " + to_string(mm_file_stat_.st_size) + "\r\n\r\n");
}

void HttpResponse::UnmapFile()
{
    if (mm_file_)
    {
        munmap(mm_file_, mm_file_stat_.st_size);
        mm_file_ = nullptr;
    }
}

string HttpResponse::GetFileType_()
{
    /* 判断文件类型 */
    string::size_type idx = path_.find_last_of('.');
    if (idx == string::npos)
    {
        return "text/plain";
    }
    string suffix = path_.substr(idx);
    if (SUFFIX_TYPE.count(suffix) == 1)
    {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

void HttpResponse::ErrorContent(Buffer &buff, string message)
{
    string body;
    string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if (CODE_STATUS.count(code_) == 1)
    {
        status = CODE_STATUS.find(code_)->second;
    }
    else
    {
        status = "Bad Request";
    }
    body += to_string(code_) + " : " + status + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>KidWebServer</em></body></html>";

    buff.Append("Content-length: " + to_string(body.size()) + "\r\n\r\n");
    buff.Append(body);
}
