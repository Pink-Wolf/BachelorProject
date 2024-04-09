using CompWolf.Doc.Server.Models;
using System.Text.Json;

namespace CompWolf.Doc.Server.Data
{
    public class ApiDatabase
    {
        public static string DataPath => "data/";

        public async Task<string?> GetProjectAsync(string project)
        {
            var path = $"{DataPath}{project}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> GetHeaderAsync(string project, string header)
        {
            var path = $"{DataPath}{project}/{header}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> GetEntityAsync(string project, string header, string name)
        {
            var path = $"{DataPath}{project}/{header}/{name}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }

        public async Task<string?> GetBriefDescriptionAsync(string path)
        {
            if (File.Exists(path) is false) return null;
            using var stream = File.OpenRead(path);
            var doc = await JsonDocument.ParseAsync(stream);
            return doc.RootElement.GetProperty("briefDescription").GetString();
        }
        public async Task<ApiCollection> GetOverviewAsync()
        {
            ApiCollection collection = new()
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
            foreach (var project in collection.Projects)
            {
                project.BriefDescription = await GetBriefDescriptionAsync($"{DataPath}{project.Name}.json") ?? "";
                foreach (var header in project.Headers)
                {
                    header.BriefDescription = await GetBriefDescriptionAsync($"{DataPath}{project.Name}/{header.Name}.json") ?? "";
                    foreach (var entity in header.Entities)
                    {
                        entity.BriefDescription = await GetBriefDescriptionAsync($"{DataPath}{project.Name}/{header.Name}/{entity.Name}.json") ?? "";
                    }
                }
            }
            return collection;
        }
    }
}
