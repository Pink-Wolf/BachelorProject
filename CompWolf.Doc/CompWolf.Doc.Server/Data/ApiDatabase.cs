using CompWolf.Doc.Server.Models;
using System.IO;
using System.Reflection.PortableExecutable;
using System.Text.Json;

namespace CompWolf.Doc.Server.Data
{
    public class ApiDatabase
    {
        private readonly static JsonSerializerOptions serializerOptions = new JsonSerializerOptions
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        };

        public static string DataPath => "data/";
        public static string ApiPath => $"{DataPath}api/";
        public static string ExamplePath => $"{DataPath}examples/";

        public async Task<string?> GetProjectAsync(string project)
        {
            var path = $"{ApiPath}{project}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> GetHeaderAsync(string project, string header)
        {
            var path = $"{ApiPath}{project}/{header}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> GetEntityAsync(string project, string header, string name)
        {
            var path = $"{ApiPath}{project}/{header}/{name}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }

        public async Task<Example?> GetExampleAsync(string name)
        {
            var path = $"{ExamplePath}{name}.json";
            if (File.Exists(path) is false) return null;
            using var stream = File.OpenRead(path);
            var example = (await JsonSerializer.DeserializeAsync<Example>(stream, serializerOptions))!;

            example.Code = await File.ReadAllTextAsync($"{ExamplePath}{example.Code}");
            return example;
        }

        public async Task<string?> GetBriefDescriptionAsync(string path)
        {
            if (File.Exists(path) is false) return null;
            using var stream = File.OpenRead(path);
            var doc = await JsonDocument.ParseAsync(stream);
            return doc.RootElement.GetProperty("briefDescription").GetString();
        }
        public async Task<SimpleApiMember[]> GetMembers(string path)
        {
            var result = Array.Empty<SimpleApiMember>();

            var entityName = Path.GetFileNameWithoutExtension(path)!;

            using var stream = File.OpenRead(path);
            var doc = await JsonDocument.ParseAsync(stream);

            bool hasConstructor = doc.RootElement.TryGetProperty("constructor", out _);

            if (doc.RootElement.TryGetProperty("memberGroups", out JsonElement memberGroups))
            {
                int memberCount = hasConstructor ? 1 : 0;
                foreach (var memberGroup in memberGroups.EnumerateArray())
                {
                    memberCount += memberGroup.GetProperty("items").GetArrayLength();
                }
                result = new SimpleApiMember[memberCount];
                int memberI = 0;

                if (hasConstructor)
                {
                    result[memberI] = new SimpleApiMember()
                    {
                        Name = entityName,
                    };
                    ++memberI;
                }

                foreach (var memberGroup in memberGroups.EnumerateArray())
                {
                    foreach (var member in memberGroup.GetProperty("items").EnumerateArray())
                    {
                        result[memberI] = new SimpleApiMember()
                        {
                            Name = member.GetProperty("name").GetString()!,
                        };

                        ++memberI;
                    }

                }
            }
            return result;
        }
        public async Task<ApiCollection> GetOverviewAsync()
        {
            ApiCollection collection = new()
            {
                Projects = Directory.GetDirectories(ApiPath).Select(projectDirectory => new SimpleApiProject()
                {
                    Name = Path.GetFileName(projectDirectory)!,
                    Headers = Directory.GetDirectories(projectDirectory).Select(headerDirectory => new SimpleApiHeader()
                    {
                        Name = Path.GetFileName(headerDirectory)!,
                        Entities = Directory.GetFiles(headerDirectory).Select(entityPath => new SimpleApiEntity()
                        {
                            Name = Path.GetFileNameWithoutExtension(entityPath)!,
                            BriefDescription = entityPath,
                        }).ToArray(),
                    }).ToArray(),
                }).ToArray(),
            };

            List<Task<SimpleApiMember[]>> tasks = [];
            foreach (var project in collection.Projects)
            {
                project.BriefDescription = await GetBriefDescriptionAsync($"{ApiPath}{project.Name}.json") ?? "";
                foreach (var header in project.Headers)
                {
                    header.BriefDescription = await GetBriefDescriptionAsync($"{ApiPath}{project.Name}/{header.Name}.json") ?? "";
                    foreach (var entity in header.Entities)
                    {
                        var filePath = entity.BriefDescription;

                        entity.BriefDescription = await GetBriefDescriptionAsync($"{ApiPath}{project.Name}/{header.Name}/{entity.Name}.json") ?? "";

                        var entityName = entity.Name;
                        tasks.Add(GetMembers(filePath));
                    }
                }
            }

            {
                var taskIterator = tasks.GetEnumerator();
                foreach (var project in collection.Projects)
                {
                    foreach (var header in project.Headers)
                    {
                        foreach (var entity in header.Entities)
                        {
                            taskIterator.MoveNext();
                            entity.Members = await taskIterator.Current;
                        }
                    }
                }
            }

            return collection;
        }
    }
}
