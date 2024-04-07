using CompWolf.Doc.Server.Models;
using System.Text.Json;

namespace CompWolf.Doc.Server.Data
{
    public class ApiDatabase
    {
        private static JsonSerializerOptions _serializerOptions = new()
        {
            PropertyNamingPolicy = JsonNamingPolicy.KebabCaseLower
        };

        public ClassApiModel? GetEntity(string project, string header, string name)
        {
            var path = $"data/{project}/{header}/{name}.json";
            if (File.Exists(path) is false) return null;
            using (FileStream reader = File.OpenRead(path))
            {
                var model = JsonSerializer.Deserialize<ClassApiModel>(reader, _serializerOptions);
                return model;
            }
        }
    }
}
