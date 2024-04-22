import { getProject, getOverview } from "@/lib/api/Entity";
import ProjectViewer from "@/lib/api/ProjectViewer";

export default async function ProjectPage({ params }) {
    let project = decodeURIComponent(params.project)

    const overview = await getOverview()
    const data = {
        ...(await getProject(project)),
        headers: overview
            .projects.find((x) => { return x.name === project })
            .headers
    }

    return <ProjectViewer data={data} />
}
