import { getOverview } from "@/lib/api/Entity";
import ClassEditor from "@/lib/api/ClassEditor";

export default async function NewClassPage() {
    const overview = await getOverview()

    return (
        <ClassEditor projects={overview.projects} />
    );
}
