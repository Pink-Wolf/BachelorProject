import { getProject, getOverview } from "@/lib/api/Entity";
import ProjectViewer from "@/lib/api/ProjectViewer";

export default async function ProjectPage({ params }) {
    const overview = await getOverview()
    const data = {
        ...(await getProject(params.project)),
        headers: overview
            .projects.find((x) => { return x.name === params.project })
            .headers
    }

    return <ProjectViewer data={data} />
}
