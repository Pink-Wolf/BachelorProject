using CompWolf.Doc.Server.Models;

namespace CompWolf.Doc.Server.Data
{
    public class ApiDatabase
    {
        public static string DataPath => "data/";

        public async Task<string?> GetEntityAsync(string project, string header, string name)
        {
            var path = $"{DataPath}{project}/{header}/{name}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }

        public async Task<string?> GetHeaderAsync(string project, string header)
        {
            var path = $"{DataPath}{project}/{header}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }

        public ApiCollection GetOverview() => new ApiCollection()
        {
            Projects = Directory.GetDirectories(DataPath).Select(projectDirectory => new SimpleApiProject()
            {
                Name = Path.GetFileName(projectDirectory)!,
                Headers = Directory.GetDirectories(projectDirectory).Select(headerDirectory => new SimpleApiHeader()
                {
                    Name = Path.GetFileName(headerDirectory)!,
                    Entities = Directory.GetFiles(headerDirectory).Select(entityPath => new SimpleApiEntity()
                    {
                        Name = Path.GetFileNameWithoutExtension(entityPath)!,
                    }).ToArray(),
                }).ToArray(),
            }).ToArray(),
        };
    }
}
