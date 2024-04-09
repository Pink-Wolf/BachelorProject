using CompWolf.Doc.Server.Data;
using CompWolf.Doc.Server.Models;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace CompWolf.Doc.Server.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class ApiController : ControllerBase
    {
        public ApiDatabase Database = new();

        [HttpGet("overview")]
        [ProducesResponseType(200)]
        public virtual ActionResult<ApiCollection> GetOverview() => Database.GetOverview();

        [HttpGet("{project}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public virtual async Task<ActionResult<string>> GetProjectAsync([FromRoute] string project)
        {
            var output = await Database.GetProjectAsync(project);
            if (output is null) return NotFound();
            return output;
        }
        [HttpGet("{project}/{header}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public virtual async Task<ActionResult<string>> GetHeaderAsync(
            [FromRoute] string project,
            [FromRoute] string header)
        {
            var output = await Database.GetHeaderAsync(project, header);
            if (output is null) return NotFound();
            return output;
        }
        [HttpGet("{project}/{header}/{name}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public virtual async Task<ActionResult<string>> GetEntityAsync(
            [FromRoute] string project,
            [FromRoute] string header,
            [FromRoute] string name)
        {
            var output = await Database.GetEntityAsync(project, header, name);
            if (output is null) return NotFound();
            return output;
        }

        [HttpPost("{project}/{header}/{name}")]
        [ProducesResponseType(201)]
        [ProducesResponseType(404)]
        public virtual async Task<ActionResult<string>> PostClass([FromBody] JsonDocument body,
            [FromRoute] string project,
            [FromRoute] string header,
            [FromRoute] string name)
        {
            var data = body.RootElement.GetRawText();
            var result = await Database.PostEntityAsync(data, project, header, name);
            if (result is false) return NotFound();
            return Created();
        }
    }
}
